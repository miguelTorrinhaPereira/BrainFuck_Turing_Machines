//TODO: OUTPUT
// high level cell = [ Marker | Data ]
#define CELL_SIZE 2
// [copy cell][curr symbol][curr state](states: (state: [state_header](actions: (action: [new state][new sym][move][empty cell]), ...)), ...)[tape barrier](tape: [tape cell], ...)

// movement cell values, also includes final states
#define LEFT_VALUE 0
#define RIGHT_VALUE 1
#define ACCEPT_VALUE 2
#define REJECT_VALUE 3
#define ABORT_VALUE 4

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

// simple loop
#define WHILE_NZ(code) "["CODE(code)"]"

// set current cell
#define _SET_CELL_ZERO() WHILE_NZ(DEC())
#define SET_CELL(value) _SET_CELL_ZERO()ADD(value)

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

#define FALSE 0
#define TRUE 1

// mark cell
#define MARK_CELL(value) SHIFT_TO_MARKER()SET_CELL(value)RET_MARKER()

// conditional
// sets the cell 2 cells to the right to 0
#define IF_EQUAL(value, code) CODE( \
	RIGHT(1) \
	SET_CELL(TRUE) \
	LEFT(1) \
	\
	SUB(value) \
	WHILE_NZ( \
		RIGHT(1) \
		SET_CELL(FALSE) \
		LEFT(1) \
	) \
	ADD(value) \
	\
	RIGHT(1) \
	WHILE_NZ( \
		SET_CELL(0) \
		LEFT(1) \
		CODE(code) \
		RIGHT(1) \
	) \
	LEFT(1) \
)

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
	WHILE_NZ( \
		GOTO_MARKER(dest_marker)INC() \
		GOTO_MARKER(orig_marker)DEC() \
	)
#define COPY(orig_marker, dest_marker) \
	MOVE(orig_marker, COPY_CELL) \
	GOTO_MARKER(dest_marker) \
	SET_CELL(0) \
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
// program
// -----------------------------


#define READ_TAPE() CODE( \
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
)


#define FIND_ACTION() CODE( \
	/* setup array search to get the curr state */ \
	/* the previous ARRAY_HEAD was the movement cell of some action */ \
	/* this is usefull for the terminal states, they can put 2 and 3 for aceptance and rejection states */ \
	GOTO_MARKER(ARRAY_HEAD) \
	MARK_CELL(UNMARKED) \
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
)

#define SIMULATE() CODE( \
	/* expected starting point for the loop, continues until the curr state is 0 */ \
	GOTO_MARKER(CURR_STATE_CELL) \
	/* main loop */ \
	WHILE_NZ( \
		FIND_ACTION()  /* marks with ARRAY_HEAD the starting cell of the correct action */ \
		\
		/* copy the next state to curr state cell */ \
		GOTO_MARKER(ARRAY_HEAD) \
		COPY(ARRAY_HEAD, CURR_STATE_CELL) \
		\
		/* go to the new sym cell of the action */ \
		MARK_CELL(ACTION_START) \
		RIGHT(1) \
		MARK_CELL(ARRAY_HEAD) \
		\
		/* copy the new sym to the tape head cell */ \
		COPY(ARRAY_HEAD, TAPE_HEAD) \
		\
		/* go to the move cell of the action */ \
		MARK_CELL(UNMARKED) \
		RIGHT(1) \
		MARK_CELL(ARRAY_HEAD) \
		\
		IF_EQUAL(LEFT_VALUE,  /* RIGHT */ \
			GOTO_MARKER(TAPE_HEAD) \
			MARK_CELL(UNMARKED) \
			RIGHT(1) \
			MARK_CELL(TAPE_HEAD) \
			GOTO_MARKER(ARRAY_HEAD) \
		) \
		IF_EQUAL(RIGHT_VALUE,  /* LFET */ \
			GOTO_MARKER(TAPE_HEAD) \
			\
			/* test to see if it is the tape barrier */ \
			LEFT(1) \
			SHIFT_TO_MARKER() \
				IF_EQUAL(TAPE_BARRIER, \
					/* this is the tape barrier! abort! */ \
					RET_MARKER() \
					/*  put the curr state to 0 so it stops */ \
					GOTO_MARKER(CURR_STATE_CELL) \
					SET_CELL(0) \
					/* put the movement to ABORT (1) to know the cause of the stop */ \
					GOTO_MARKER(ARRAY_HEAD) \
					SET_CELL(ABORT_VALUE) \
					\
					GOTO_MARKER(TAPE_BARRIER) \
					SHIFT_TO_MARKER() \
				) \
			RET_MARKER() \
			\
			/* actually do the left movement */ \
			/* The IF_EQUAL already set the old head to 0 */ \
			MARK_CELL(TAPE_HEAD) \
		) \
		\
		/* if the state is 0, the machine stops */ \
		GOTO_MARKER(CURR_STATE_CELL) \
	) \
)

#define DO_ACCEPT() CODE( \
	   INC() \
)

#define DO_REJECT() CODE( \
	   INC() \
)

#define DO_ABORT() CODE( \
	   INC() \
)

//TODO: remove the if else logic, unecessary
#define OUTPUT_RESULT() CODE(\
	GOTO_MARKER(CURR_STATE_CELL)  /* wouldn't be used anymore */ \
	SET_CELL(TRUE) \
	\
	/* If, else if, else if statment */ \
	WHILE_NZ( \
		GOTO_MARKER(ARRAY_HEAD) \
		IF_EQUAL(ABORT_VALUE, DO_ABORT()) \
		GOTO_MARKER(CURR_STATE_CELL) \
		SET_CELL(FALSE) \
	) \
	WHILE_NZ( \
		GOTO_MARKER(ARRAY_HEAD) \
		IF_EQUAL(ACCEPT_VALUE, DO_ACCEPT()) \
		GOTO_MARKER(CURR_STATE_CELL) \
		SET_CELL(FALSE) \
	) \
	WHILE_NZ( \
		GOTO_MARKER(ARRAY_HEAD) \
		IF_EQUAL(REJECT_VALUE, DO_REJECT()) \
	) \
)

#define TURING_MACHINE_BF_PROGRAM CODE( \
	READ_TAPE() \
	SIMULATE() \
	OUTPUT_RESULT() \
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
