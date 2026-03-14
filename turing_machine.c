#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "segment_array.h"


#define MAX_LINE_SIZE ((1 << 12) + 1)
#define MAX_STATE_STR_SIZE 32
#define UNUSED_CHAR 0
#define RESERVED_STATES_COUNT 3
#define MAX_EXTRA_STATE_COUNT (UINT16_MAX - RESERVED_STATES_COUNT)
#define MA_VALID_STATE_NUM (UINT16_MAX - (RESERVED_STATES_COUNT - 1))
#define FAILURE -1
#define SUCCESS 0
#define HEAD_CHAR 'V'
#define BLANCK_CHAR '_'

enum : uint16_t {
	Qin,
	Qac,
	Qrej
};


typedef enum : int8_t {
	L = -1,
	S = 0,
	R = 1
} direction_t;

typedef struct {
	uint8_t new_symbol;
	direction_t direction;
	uint16_t new_state;
} action_t;


const uint8_t blanck_symbol = 0;
const char *const reserved_states_str[RESERVED_STATES_COUNT] = {"qin", "qac", "qrej"};
uint32_t alphabet_size = 1;  // black symbol is char □
uint8_t char_to_symbol[UCHAR_MAX];
char symbol_to_char[UCHAR_MAX];  // certanlly, there aren't more symbols than input characters


uint32_t get_curr_tape_head_pos(int64_t head_pos) {
	if(head_pos >= 0)
		return head_pos;
	else
		return -head_pos - 1;
}


segment_array_t get_curr_tape(int64_t head_pos, segment_array_t tape_left, segment_array_t tape_right) {
	return (head_pos >= 0) ? tape_right : tape_left;
}


int32_t get_state_num(const char *state_str) {
	// check for reserved states
	for(int i = 0; i < RESERVED_STATES_COUNT; i++)
		if(strcmp(state_str, reserved_states_str[i]) == 0)
			return i;

	// ignore leading q
	if(state_str[0] == 'q')
		state_str++;

	char *first_invalid_char = NULL;
	long state_raw = strtol(state_str, &first_invalid_char, 10);
	if(*first_invalid_char != '\0' || state_raw > MA_VALID_STATE_NUM)
		return FAILURE;

	// if it is <= MA_VALID_STATE_NUM it certanly fits in a int32_t
	return (int32_t) state_raw;
}


// it doesn't chec, the input, I don't care
void read_state_actions(const char *state_str, action_t state_actions[alphabet_size]) {
	printf("%s:\n", state_str);

	for(int i = 0; i < alphabet_size; i++) {
		printf("	%c -> ", symbol_to_char[i]);

		uint8_t new_char = getchar();
		if(new_char == '\n') {
			state_actions[i] = (action_t){ symbol_to_char[i], S, Qrej };
			continue;
		}
		ungetc(new_char, stdin);

		scanf(" %c ,", &new_char);
		state_actions[i].new_symbol = char_to_symbol[new_char];

		scanf(" %c ,", &new_char);
		new_char = toupper(new_char);
		switch(new_char) {
			case 'L':
				state_actions[i].direction = L;
				break;
			case 'S':
				state_actions[i].direction = S;
				break;
			case 'R':
				state_actions[i].direction = R;
				break;
		}

		char new_state_str[MAX_STATE_STR_SIZE];
		scanf(" %s", new_state_str);
		state_actions[i].new_state = get_state_num(new_state_str);
	}
}


void print_machine_state(int64_t head_pos, segment_array_t tape_left, segment_array_t tape_right) {
	uint64_t head_space = 0;
	head_space++;  // left □
	head_space += sa_size(tape_left);
	head_space += (head_pos >= 0) ? get_curr_tape_head_pos(head_pos) : -(get_curr_tape_head_pos(head_pos) + 1);
	while(head_space--)
		putchar(' ');
	putchar(HEAD_CHAR);
	putchar('\n');

	putchar(BLANCK_CHAR);
	int64_t pos = sa_size(tape_left) - 1;  // needs to be negative
	uint32_t limit = 0;
	while(pos >= 0)
		putchar(symbol_to_char[*(uint8_t *)sa_get(tape_left, pos--)]);
	pos = 0;
	limit = sa_size(tape_right) - 1;
	while(pos <= limit)
		putchar(symbol_to_char[*(uint8_t *)sa_get(tape_right, pos++)]);
	putchar(BLANCK_CHAR);
	printf("\n\n");
}


int main()
{
	memset(char_to_symbol, UNUSED_CHAR, UCHAR_MAX);
	symbol_to_char[0] = BLANCK_CHAR;

	char line[MAX_LINE_SIZE];

	printf("alphabet: ");
	fgets(line, MAX_LINE_SIZE, stdin);

	const char *line_p = line;
	while(*line_p != '\n') {
		const char new_char = *line_p;

		if(isspace(new_char) || char_to_symbol[new_char] != UNUSED_CHAR) {
			line_p++;
			continue;
		}
	
		char_to_symbol[new_char] = alphabet_size;
		symbol_to_char[char_to_symbol[new_char]] = new_char;
		alphabet_size++;

		line_p++;
	}

	uint16_t extra_state_count;
	printf("extra states count: ");
	scanf(" %hu", &extra_state_count);
	getchar();  // skip trailing \n
	uint16_t state_count = RESERVED_STATES_COUNT + extra_state_count;

	action_t state_actions[state_count][alphabet_size];
	read_state_actions("qin", state_actions[0]);
	for(int i = RESERVED_STATES_COUNT; i < state_count; i++) {
		char state_str[MAX_STATE_STR_SIZE];
		state_str[0] = 'q';
		sprintf(state_str + 1, "%u", i - (RESERVED_STATES_COUNT - 1));  // it is 1 based, not 0

		read_state_actions(state_str, state_actions[i]);
	}
	
	int64_t head_pos = 0;
	uint16_t state = 0;
	segment_array_t tape_left = sa_create(sizeof(uint8_t));
	sa_push_back(tape_left, &blanck_symbol);
	segment_array_t tape_right = sa_create(sizeof(uint8_t));
	sa_push_back(tape_right, &blanck_symbol);

	while(state != Qac || state != Qrej) {
		segment_array_t curr_tape = get_curr_tape(head_pos, tape_left, tape_right);
		uint32_t curr_tape_head_pos = get_curr_tape_head_pos(head_pos);
		uint8_t symbol = *(uint8_t *)sa_get(curr_tape, curr_tape_head_pos);
		action_t action = state_actions[state][symbol];

		sa_switch(curr_tape, curr_tape_head_pos, &action.new_symbol, NULL);

		state = action.new_state;
		head_pos += action.direction;

		curr_tape = get_curr_tape(head_pos, tape_left, tape_right);
		curr_tape_head_pos = get_curr_tape_head_pos(head_pos);
		if(curr_tape_head_pos > sa_capacity(curr_tape))
			if(sa_push_back(curr_tape, &blanck_symbol) != SA_SUCCESS)
				exit(1);
		
		print_machine_state(head_pos, tape_left, tape_right);
	}

	printf("final state: ");
	puts(reserved_states_str[state]);
}
