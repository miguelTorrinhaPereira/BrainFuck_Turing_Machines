#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "segment_array.h"
#include "bf_values.h"


#define MAX_LINE_SIZE ((1 << 12) + 1)
#define MAX_STATE_STR_SIZE 32
#define UNUSED_CHAR 0
#define MAX_EXTRA_STATE_COUNT (UINT8_MAX - RESERVED_STATES_COUNT)
#define MAX_VALID_STATE_NUM (UINT8_MAX - (RESERVED_STATES_COUNT - 1))
#define BLANCK_CHAR '_'

#define INITIAL_CELLS_COUNT (STATE_START - 1)
#define ACTION_CELL_COUNT 3
#define OUTPUT_END_SYM '\xff'  // 255


typedef uint8_t state_num_t;
enum : state_num_t {
	qnull,
	qin,
	qac,
	qrej,
	RESERVED_STATES_COUNT
};

typedef struct {
	uint8_t new_state;
	uint8_t new_symbol;
	uint8_t direction;
} action_t;


const uint8_t blanck_symbol = 0;
const char *const reserved_states_str[RESERVED_STATES_COUNT] = {"qnull", "qin", "qac", "qrej"};
uint32_t alphabet_size = 1;  // black symbol is char _
uint8_t char_to_symbol[UCHAR_MAX];
char symbol_to_char[UCHAR_MAX];  // certanlly, there aren't more symbols than input characters


const char *get_state_str(state_num_t state) {
	if (state < RESERVED_STATES_COUNT)
		return reserved_states_str[state];

	static char state_str[MAX_STATE_STR_SIZE];
	state_str[0] = 'q';
	sprintf(state_str + 1, "%u", state - (RESERVED_STATES_COUNT - 1));  // it is 1 based, not 0

	return state_str;
}


state_num_t get_state_num(const char *state_str) {
	// ignore leading q
	if (tolower(state_str[0]) == 'q')
		state_str++;

	// check for reserved states
	for (int i = 0; i < RESERVED_STATES_COUNT; i++)
		if (strcmp(state_str, reserved_states_str[i] + 1) == 0)  // + 1 to skip the leading q
			return i;

	char *first_invalid_char = NULL;
	long state_raw = strtol(state_str, &first_invalid_char, 10) + (RESERVED_STATES_COUNT - 1);

	return (state_num_t) state_raw;
}


// it doesn't check the input, I don't care
void read_state_actions(const char *state_str, action_t state_actions[alphabet_size]) {
	char new_state_str[MAX_STATE_STR_SIZE];
	printf("%s:\n", state_str);

	for (int i = 0; i < alphabet_size; i++) {
		printf("	%c -> ", symbol_to_char[i]);

		uint8_t new_char = getchar();
		if (new_char == '\n') {
			state_actions[i] = (action_t){ .new_state = qrej, .new_symbol = i, .direction = STAY_VALUE };
			continue;
		}
		ungetc(new_char, stdin);

		uint8_t new_sym_raw, direction_raw;
		scanf(" %c , %c , %s", &new_sym_raw, &direction_raw, new_state_str);
		getchar();  // get trailing \n

		state_actions[i].new_symbol = char_to_symbol[new_sym_raw];

		direction_raw = toupper(direction_raw);
		switch(direction_raw) {
			case 'L':
				state_actions[i].direction = LEFT_VALUE;
				break;
			case 'S':
				state_actions[i].direction = STAY_VALUE;
				break;
			case 'R':
				state_actions[i].direction = RIGHT_VALUE;
				break;
		}

		state_actions[i].new_state = get_state_num(new_state_str);
	}
}


void setup_final_state_actions(action_t state_actions[alphabet_size], uint8_t move_value) {
	for(int i = 0; i < alphabet_size; i++)
		state_actions[i] = (action_t){.new_state = TERMINAL_STATE, .new_symbol = i, .direction = move_value};
}


int main()
{
	memset(char_to_symbol, UNUSED_CHAR, UCHAR_MAX);
	symbol_to_char[0] = BLANCK_CHAR;

	char line[MAX_LINE_SIZE];

	printf("alphabet: ");
	fgets(line, MAX_LINE_SIZE, stdin);

	const char *line_p = line;
	while (*line_p != '\n') {
		const char new_char = *line_p;

		if (isspace(new_char) || char_to_symbol[new_char] != UNUSED_CHAR) {
			line_p++;
			continue;
		}
	
		char_to_symbol[new_char] = alphabet_size++;
		symbol_to_char[char_to_symbol[new_char]] = new_char;

		line_p++;
	}

	state_num_t extra_state_count;
	printf("extra states count: ");
	scanf(" %hhu", &extra_state_count);
	getchar();  // skip trailing \n
	state_num_t state_count = RESERVED_STATES_COUNT + extra_state_count;

	action_t state_actions[state_count][alphabet_size];
	setup_final_state_actions(state_actions[qac], ACCEPT_VALUE);
	setup_final_state_actions(state_actions[qrej], REJECT_VALUE);

	read_state_actions(get_state_str(qin), state_actions[qin]);
	for (int i = RESERVED_STATES_COUNT; i < state_count; i++) {
		read_state_actions(get_state_str(i), state_actions[i]);
	}

	uint32_t state_cell_count = 1 + ACTION_CELL_COUNT * alphabet_size;  // don't forget the header
	uint32_t states_cell_count = state_count * state_cell_count;
	uint32_t translation_table_cell_count = 1 + alphabet_size;  // don't forget the header
	uint32_t rw_tape_size = INITIAL_CELLS_COUNT + states_cell_count + translation_table_cell_count + 1;  // tape barrier
	
	uint8_t markers[rw_tape_size] = {};
	// in this case, the marker -1 corresponds to the cell position
	markers[COPY_CELL - 1] = COPY_CELL;
	markers[CONDITION_CELL - 1] = CONDITION_CELL;
	markers[CURR_STATE_CELL - 1] = CURR_STATE_CELL;
	markers[CURR_SYM_CELL - 1] = CURR_SYM_CELL;
	markers[INITIAL_CELLS_COUNT + 2] = ARRAY_HEAD;
	for(int i = 0; i < state_count; i++) {
		uint32_t state_start = INITIAL_CELLS_COUNT + i * state_cell_count;
		markers[state_start] = STATE_START;

		for(int j = 0; j < alphabet_size; j++) {
			uint32_t action_start = (state_start + 1) + j * ACTION_CELL_COUNT;
			markers[action_start] = ACTION_START;
		}
	}
	markers[INITIAL_CELLS_COUNT + states_cell_count] = TRANSLATION_TABLE;
	markers[INITIAL_CELLS_COUNT + states_cell_count + translation_table_cell_count] = TAPE_BARRIER;

	uint8_t data[rw_tape_size] = {};
	data[CURR_STATE_CELL - 1] = qin;
	for(int i = 1; i < state_count; i++) {
		uint32_t state_start = INITIAL_CELLS_COUNT + i * state_cell_count;
		data[state_start] = i;

		for(int j = 0; j < alphabet_size; j++) {
			uint32_t action_start = (state_start + 1) + j * ACTION_CELL_COUNT;
			action_t action = state_actions[i][j];
			data[action_start + 0] = action.new_state;
			data[action_start + 1] = action.new_symbol;
			data[action_start + 2] = action.direction;
		}
	}
	uint32_t translation_table_start = INITIAL_CELLS_COUNT + states_cell_count;
	for(int i = 0; i < alphabet_size; i++)
		data[translation_table_start + 1 + i] = symbol_to_char[i] - BF_INPUT_OFFSET;  // they are already printable


	segment_array_t input_tape = sa_create(sizeof(uint8_t));
	while(true) {
		sa_clear(input_tape);

		printf("\ninput: ");

		int new_char = getchar();
		if (new_char == '\n' || new_char == EOF)
			break;

		do {
			sa_push_back(input_tape, &char_to_symbol[new_char]);
			new_char = getchar();
		} while (new_char != '\n' && new_char != EOF);

		// rw tape
		for(int i = 0; i < rw_tape_size; i++) {
			putchar(markers[i] + BF_INPUT_OFFSET);
			putchar(data[i] + BF_INPUT_OFFSET);
		}

		// input tape
		putchar(TAPE_HEAD + BF_INPUT_OFFSET);
		putchar(*(uint8_t *)sa_get(input_tape, 0) + BF_INPUT_OFFSET);
		for(int i = 1; i < sa_size(input_tape); i++) {
			putchar(UNMARKED + BF_INPUT_OFFSET);
			putchar(*(uint8_t *)sa_get(input_tape, i) + BF_INPUT_OFFSET);
		}

		putchar(0 + BF_INPUT_OFFSET);  // so that the machine reads the output end sym in a data cell
		putchar(OUTPUT_END_SYM + BF_INPUT_OFFSET);
		putchar('\n');
	}

	sa_delete(input_tape);
}
