#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include "segment_array.h"


#define MAX_LINE_SIZE ((1 << 12) + 1)
#define UNUSED_CHAR 0
#define RESERVED_STATES_COUNT 3
#define MAX_EXTRA_STATE_COUNT (UINT16_MAX - RESERVED_STATES_COUNT)
#define MA_VALID_STATE_NUM (UINT16_MAX - (RESERVED_STATES_COUNT - 1))
#define FAILURE -1
#define SUCCESS 0
#define HEAD_CHAR L'▽'
#define BLANCK_CHAR L'□'

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
	uint8_t new_simbol;
	direction_t direction;
	uint16_t new_state;
} action_t;


const uint8_t blanck_simbol = 0;
const char *const reserved_states_str[RESERVED_STATES_COUNT] = {"qin", "qac", "qrej"};
uint32_t alphabet_size = 1;  // black symbol is char □
uint8_t char_to_simbol[CHAR_MAX];
wchar_t simbol_to_char[CHAR_MAX];  // certanlly, there aren't more simbols than input characters


inline uint32_t get_curr_tape_head_pos(int64_t head_pos) {
	if(head_pos >= 0)
		return head_pos;
	else
		return -head_pos - 1;
}


inline segment_array_t get_curr_tape(int64_t head_pos, segment_array_t tape_left, segment_array_t tape_right) {
	return (head_pos >= 0) ? tape_right : tape_left;
}


int32_t read_state(const char *state_str) {
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


// can't just expect 
// return  is just used to say if it failed
int32_t read_state_actions(const char *state_str, action_t state_actions[alphabet_size]) {
}


//TODO: implement this shit
void print_machine_state(int64_t head_pos, segment_array_t tape_left, segment_array_t tape_right) {
	uint64_t head_space = 0;
	head_space++;  // left □
	head_pos += sa_size(tape_left);
	head_pos += (head_pos >= 0) ? get_curr_tape_head_pos(head_pos) : -(get_curr_tape_head_pos(head_pos) + 1);
	while(head_space--)
		putchar(' ');
	putwchar(HEAD_CHAR);
	putchar('\n');

	putwchar(BLANCK_CHAR);
	uint32_t pos = sa_size(tape_left) - 1;
	uint32_t limit = 0;
	while(pos >= 0)
		putwchar(simbol_to_char[*(uint8_t *)sa_get(tape_left, pos--)]);
	pos = 0;
	limit = sa_size(tape_right);
	while(pos < limit)
		putwchar(simbol_to_char[*(uint8_t *)sa_get(tape_right, pos++)]);
	putwchar(BLANCK_CHAR);
	printf("\n\n");
}


int main()
{
	memset(char_to_simbol, UNUSED_CHAR, CHAR_MAX);
	simbol_to_char[0] = L'□';

	char line[MAX_LINE_SIZE];

	printf("alphabet: ");
	fgets(line, MAX_LINE_SIZE, stdin);

	char *line_p = line;
	while(*line_p != '\n') {
		char new_char = *line_p;

		if(isspace(new_char) || char_to_simbol[new_char] != UNUSED_CHAR) {
			line_p++;
			continue;
		}
	
		char_to_simbol[new_char] = alphabet_size + 1;
		simbol_to_char[char_to_simbol[new_char]] = new_char;
		alphabet_size++;

		line_p++;
	}

	uint16_t state_count;
	printf("extra states count: ");
	scanf(" %hu", &state_count);

	action_t state_actions[state_count][alphabet_size];
	read_state_actions("qin", state_actions[0]);
	for(int i = RESERVED_STATES_COUNT; i < state_count; i++) {
		char state_str[32];
		state_str[0] = 'q';
		sprintf(state_str + 1, "%u", i - (RESERVED_STATES_COUNT - 1));  // it is 1 based, not 0

		read_state_actions(state_str, state_actions[i]);
	}
	
	setlocale(LC_ALL, "en_US.UTF-8");  // for printing □
	int64_t head_pos = 0;
	uint16_t state = 0;
	segment_array_t tape_left = sa_create(sizeof(uint8_t));
	segment_array_t tape_right = sa_create(sizeof(uint8_t));

	while(state != Qac || state != Qrej) {
		segment_array_t curr_tape = get_curr_tape(head_pos, tape_left, tape_right);
		uint32_t curr_tape_head_pos = get_curr_tape_head_pos(head_pos);
		uint8_t simbol = *(uint8_t *)sa_get(curr_tape, curr_tape_head_pos);
		action_t action = state_actions[state][simbol];

		sa_switch(curr_tape, curr_tape_head_pos, &action.new_simbol, NULL);

		state = action.new_state;
		head_pos += action.direction;

		curr_tape = get_curr_tape(head_pos, tape_left, tape_right);
		curr_tape_head_pos = get_curr_tape_head_pos(head_pos);
		if(curr_tape_head_pos > sa_capacity(curr_tape))
			if(sa_push_back(curr_tape, &blanck_simbol) != SA_SUCCESS)
				exit(1);
		
		print_machine_state(head_pos, tape_left, tape_right);
	}

	printf("final state: ");
	puts(reserved_states_str[state]);
}
