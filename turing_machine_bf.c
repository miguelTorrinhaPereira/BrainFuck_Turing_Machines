//TODO: OUTPUT
// high level cell = [ Marker | Data ]
#define CELL_SIZE 2
// [copy cell][curr symbol][curr state](states: (state: [state_header](actions: (action: [new state][new sym][move]), ...)), ...)[tape barrier](tape: [tape cell], ...)

// used to avoid spaces
#define CODE(code) code

// repetition
#define REPEAT_0(code)
#define REPEAT_1(code) code
#define REPEAT_2(code) code code
#define REPEAT_3(code) code code code
#define REPEAT_4(code) code code code code
#define REPEAT_5(code) code code code code code
#define REPEAT_6(code) code code code code code code
#define REPEAT_7(code) code code code code code code code
#define REPEAT_8(code) code code code code code code code code
#define REPEAT_9(code) code code code code code code code code code
#define REPEAT_(count, code) REPEAT_##count(code)
#define REPEAT(count, code) REPEAT_(count, code)

// arithmetic
#define INC() "+"
#define DEC() "-"
#define ADD(value) REPEAT(value, INC())
#define SUB(value) REPEAT(value, DEC())

// conditional
#define WHILE_NZ(code) "["CODE(code)"]"

// set current cell
#define _SET_ZERO() WHILE_NZ(DEC())
#define SET(value) _SET_ZERO()ADD(value)

// movement
#define _LEFT() "<"
#define _RIGHT() ">"
#define LEFT(count) REPEAT(count, REPEAT(CELL_SIZE, _LEFT()))
#define RIGHT(count) REPEAT(count, REPEAT(CELL_SIZE, _RIGHT()))

// [ Marker | Data ]
#define SHIFT_TO_MARKER() _LEFT()
#define RET_MARKER() _RIGHT()

// marker values
#define UNMARKED 0
#define COPY_CELL 1
#define CURR_SYM_CELL 2
#define CURR_STATE_CELL 3
#define STATE_START 4
#define ACTION_START 5
#define ARRAY_HEAD 6
#define TAPE_HEAD 7
#define TAPE_BARRIER 8

// set mark
#define MARK_CELL(value) SHIFT_TO_MARKER()SET(value)RET_MARKER()
#define UNMARK_CELL() MARK_CELL(UNMARKED)

// find marker
#define _GOTO_MARKER_L(marker) \
	SHIFT_TO_MARKER() \
		SUB(marker) \
		WHILE_NZ(ADD(marker)LEFT(1)SUB(marker)) \
		ADD(marker) \
	RET_MARKER()
#define _GOTO_MARKER_R(marker) \
	SHIFT_TO_MARKER() \
		SUB(marker) \
		WHILE_NZ(ADD(marker)RIGHT(1)SUB(marker)) \
		ADD(marker) \
	RET_MARKER()
#define GOTO_MARKER(marker) _GOTO_MARKER_L(COPY_CELL) _GOTO_MARKER_R(marker)

// data movement
// doesn't celan the dest cell
// assumes it is starting in the orig cell
#define MOVE(orig_marker, dest_marker) \
	GOTO_MARKER(dest_marker) \
	SET(0) \
	WHILE_NZ( \
		GOTO_MARKER(dest_marker)INC() \
		GOTO_MARKER(orig_marker)DEC() \
	)
#define COPY(orig_marker, dest_marker) \
	MOVE(orig_marker, COPY_CELL) \
	GOTO_MARKER(dest_marker) \
	SET(0) \
	_GOTO_MARKER_L(COPY_CELL) \
	WHILE_NZ( \
		_GOTO_MARKER_R(dest_marker)INC() \
		GOTO_MARKER(orig_marker)INC() \
		_GOTO_MARKER_L(COPY_CELL)DEC() \
	) \
	_GOTO_MARKER_R(orig_marker)  // finish in the orig cell

// array search
#define ARRAY_SEARCH(index_marker, element_marker) \
	WHILE_NZ( \
		GOTO_MARKER(ARRAY_HEAD)MARK_CELL(element_marker) \
		_GOTO_MARKER_R(element_marker)MARK_CELL(ARRAY_HEAD) \
		GOTO_MARKER(index_marker)DEC() \
	)

// IO
#define INPUT() ","
#define PRINT() "."


// -----------------------------
// main function
// -----------------------------


#define TURING_MACHINE_BF_PROGRAM CODE( \
	/* input */ \
	/* first cell */ \
	INPUT() \
	/* read input until you read a 255 */ \
	INC() \
	WHILE_NZ( \
		DEC() \
		/* we need to write every cell, not just the data cells */ \
		_RIGHT() \
		INPUT() \
		INC() \
	) \
	/* leave it at 0, this will be used as tape */ \
	\
	/* expected starting point for the loop, continues until the curr state is 0 */ \
	GOTO_MARKER(CURR_STATE_CELL) \
	\
	/* main loop */ \
	WHILE_NZ( \
		/* setup array search to get the curr state */ \
		/* the previous ARRAY_HEAD was the movement cell of some action */ \
		/* this is usefull for the terminal states, they can put 2 and 3 for aceptance and rejection states */ \
		GOTO_MARKER(ARRAY_HEAD) \
		UNMARK_CELL() \
		/* go to the first cell of the first state */ \
		GOTO_MARKER(STATE_START) \
		MARK_CELL(ARRAY_HEAD) \
		/* go back to the curr state cell */ \
		GOTO_MARKER(CURR_STATE_CELL) \
		/* the index cell end with the value 0 */ \
		ARRAY_SEARCH(CURR_STATE_CELL, STATE_START) \
		\
		/* copy the curr symbol of the turing machine */ \
		GOTO_MARKER(TAPE_HEAD) \
		COPY(TAPE_HEAD, CURR_SYM_CELL) \
		GOTO_MARKER(CURR_SYM_CELL) \
		\
		/* get the correct action, we are already have marked the correct state */ \
		GOTO_MARKER(ARRAY_HEAD) \
		/* skip state header */ \
		MARK_CELL(STATE_START) \
		RIGHT(1) \
		MARK_CELL(ARRAY_HEAD) \
		GOTO_MARKER(CURR_SYM_CELL) \
		ARRAY_SEARCH(CURR_SYM_CELL, ACTION_START) \
		\
		/* copy the next state to curr state cell */ \
		GOTO_MARKER(ARRAY_HEAD) \
		COPY(ARRAY_HEAD, CURR_STATE_CELL) \
		\
		/* go to the new sym cell of the action */ \
		UNMARK_CELL() \
		RIGHT(1) \
		MARK_CELL(ARRAY_HEAD) \
		\
		/* copy the new sym to the tape head cell */ \
		COPY(ARRAY_HEAD, TAPE_HEAD) \
		\
		/* go to the move cell of the action */ \
		UNMARK_CELL() \
		RIGHT(1) \
		MARK_CELL(ARRAY_HEAD) \
		\
		/* move is 1 for right, 0 for left */ \
		WHILE_NZ( \
			/* we need to move it to the right, so move the head to the right 2 times */ \
			/* latter it will be moved it to the left one time */ \
			GOTO_MARKER(TAPE_HEAD) \
			UNMARK_CELL() \
			RIGHT(2) \
			MARK_CELL(TAPE_HEAD) \
			GOTO_MARKER(ARRAY_HEAD) \
		) \
		GOTO_MARKER(TAPE_HEAD) \
		\
		/* test to see if it is the tape barrier */ \
		MARK_CELL(1)  /* TRUE */ \
		LEFT(1) \
		SHIFT_TO_MARKER() \
			SUB(TAPE_BARRIER) \
			WHILE_NZ( \
				/* it isn't the tape barrier */ \
				RIGHT(1) \
				MARK_CELL(0)  /* FALSE */ \
				LEFT(1) \
			) \
			ADD(TAPE_BARRIER) \
			RIGHT(1) \
			WHILE_NZ( \
				/* this is the tape barrier! abort! */ \
				RET_MARKER() \
				/*  put the curr state to 0 so it stops */ \
				GOTO_MARKER(CURR_STATE_CELL) \
				SET(0) \
				/* put the movement to 0 to know the cause of the stop */ \
				GOTO_MARKER(ARRAY_HEAD) \
				SET(0) \
				\
				GOTO_MARKER(TAPE_BARRIER) \
				RIGHT(1) \
				SHIFT_TO_MARKER() \
			) \
			LEFT(1)  /* added for readability */ \
		RET_MARKER() \
		RIGHT(1) \
		UNMARK_CELL() \
		\
		/* actually do the left movement */ \
		LEFT(1) \
		MARK_CELL(TAPE_HEAD) \
		\
		/* if the state is 0, the machine stops */ \
		GOTO_MARKER(CURR_STATE_CELL) \
	) \
	\
	\
	/* output */ \
	/* the ARRAY_HEAD marker is in the movement cell */ \
	GOTO_MARKER(ARRAY_HEAD) \
	\
	\
	\
)


#include <stdio.h>
#include <string.h>
#include <stdbool.h>


const char *get_op_pair(char c) {
	switch (c) {
		case '<': case '>':
			return "<>";
		case '+': case '-':
			return "+-";
		case '[':
			return "[";
		case ']':
			return "]";
		case ',':
			return ",";
		case '.':
			return ".";
	}

	return NULL;
}


int main() {  // just cleans up the string, remove operations that cancel each other
	char buffers[2][sizeof(TURING_MACHINE_BF_PROGRAM)];
	char *raw_bf = buffers[0], *processed_bf = buffers[1];
	strcpy(raw_bf, TURING_MACHINE_BF_PROGRAM);
	processed_bf[0] = '\0';

	const char *curr_op_pair = "";
	int curr_count = 0;
	char needle_str[2] = {};

	while (true) {
		char *processed_bf_p = processed_bf;
		for(char *c_p = raw_bf; *c_p != '\0'; c_p++) {
			needle_str[0] = *c_p;

			if(strstr(curr_op_pair, needle_str) == NULL) {  // is not present
				if(strcmp(curr_op_pair, "") != 0) {  // isn't empty
					char sym = curr_op_pair[curr_count < 0];
					int count = (curr_count < 0) ? -curr_count : curr_count;
					
					for(int i = 0; i < count; i++)
						*(processed_bf_p++) = sym;
				}

				curr_op_pair = get_op_pair(*c_p);
				curr_count = 0;
			}

			curr_count += 1 - 2 * (strstr(curr_op_pair, needle_str) - curr_op_pair);
		}

		if(strcmp(curr_op_pair, "") != 0) {  // isn't empty
			char sym = curr_op_pair[curr_count < 0];
			int count = (curr_count < 0) ? -curr_count : curr_count;

			for(int i = 0; i < count; i++)
				*(processed_bf_p++) = sym;
		}
		*processed_bf_p = '\0';

		if (strlen(processed_bf) != strlen(raw_bf)) {
			char *tmp = processed_bf;
			processed_bf = raw_bf;
			raw_bf = tmp;
			processed_bf[0] = '\0';

			curr_op_pair = "";
			curr_count = 0;
		}
		else
			break;
	}

	printf("%s\n", processed_bf);
}
