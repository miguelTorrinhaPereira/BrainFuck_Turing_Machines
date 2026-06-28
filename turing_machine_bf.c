// high level cell = [ Marker | Data ]
#define CELL_SIZE 2
// [copy cell][curr symbol][curr state](states: (state: [state_header](actions: (action: [new state][new sym][move][empty cell]), ...)), ...)
// [translation table header](translation table: [translation], ...)
// [tape barrier](tape: [tape cell], ...)

// movement cell values, also includes final states
#define LEFT_VALUE 0
#define RIGHT_VALUE 1
#define ACCEPT_VALUE 2
#define REJECT_VALUE 3
#define ABORT_VALUE 4

// used to avoid spaces
#define CODE(code) code

// variadict macro stuff
#define _ARGS_COUNT_NUMS() 5, 4, 3, 2, 1, 0
#define __ARGS_COUNT(_1, _2, _3, _4, _5, count, ...) count
#define _ARGS_COUNT(...) __ARGS_COUNT(__VA_ARGS__)
#define ARGS_COUNT(...) _ARGS_COUNT(__VA_ARGS__ __VA_OPT__(,) _ARGS_COUNT_NUMS())

// concat macros
#define _CONCAT_CALL(macro, value, ...) macro##value(__VA_ARGS__)
#define CONCAT_CALL(macro, value, ...) _CONCAT_CALL(macro, value, __VA_ARGS__)

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
#define REPEAT_10(code) code code code code code code code code code code
#define REPEAT_11(code) code code code code code code code code code code code
#define REPEAT_12(code) code code code code code code code code code code code code
#define REPEAT_13(code) code code code code code code code code code code code code code
#define REPEAT_14(code) code code code code code code code code code code code code code code
#define REPEAT_15(code) code code code code code code code code code code code code code code code
#define REPEAT_16(code) code code code code code code code code code code code code code code code code
#define REPEAT_17(code) code code code code code code code code code code code code code code code code code
#define REPEAT_18(code) code code code code code code code code code code code code code code code code code code
#define REPEAT_19(code) code code code code code code code code code code code code code code code code code code code
#define REPEAT_20(code) code code code code code code code code code code code code code code code code code code code code
#define REPEAT_(count, code) REPEAT_##count(code)
#define REPEAT(count, code) REPEAT_(count, code)

// arithmetic
#define INC() "+"
#define DEC() "-"
#define ADD(value) REPEAT(value, INC())
#define SUB(value) REPEAT(value, DEC())

#define _DO_OP_FACTORS_1(op, value) op(value)
#define _DO_OP_FACTORS_2(op, value, ...) ADD(value) WHILE_NZ(LEFT() _DO_OP_FACTORS_1(op, __VA_ARGS__) RIGHT()DEC())
#define _DO_OP_FACTORS_3(op, value, ...) ADD(value) WHILE_NZ(LEFT() _DO_OP_FACTORS_2(op, __VA_ARGS__) RIGHT()DEC())
#define _DO_OP_FACTORS_4(op, value, ...) ADD(value) WHILE_NZ(LEFT() _DO_OP_FACTORS_3(op, __VA_ARGS__) RIGHT()DEC())

//WARNING: all aux cell to the right must be with the value 0 for it to work properly
// should only be used with ADD and SUB
// It is smaller for more than 20 operations
#define DO_OP_FACTORS(op, value, ...) \
	REPEAT(ARGS_COUNT(__VA_ARGS__), RIGHT()) \
	CONCAT_CALL(_DO_OP_FACTORS_, ARGS_COUNT(value, __VA_ARGS__), op, value __VA_OPT__(,) __VA_ARGS__)  /* compute the value */ \
	REPEAT(ARGS_COUNT(__VA_ARGS__), LEFT())  /* go to the cell with the final value */

// simple loop
#define WHILE_NZ(code) "["code"]"

// set  cell
#define _SET_CELL_ZERO() WHILE_NZ(DEC())
#define SET_CELL(value) _SET_CELL_ZERO()ADD(value)

#define SET_MULTIPLE_CELLS(value, count) \
	REPEAT(count, RIGHT()) \
	REPEAT(count, LEFT()SET_CELL(value))


// movement
#define _LEFT() "<"
#define _RIGHT() ">"
#define LEFT() REPEAT(CELL_SIZE, _LEFT())
#define RIGHT() REPEAT(CELL_SIZE, _RIGHT())
#define LEFT_N(count) REPEAT(count, LEFT())
#define RIGHT_N(count) REPEAT(count, RIGHT())

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
#define TRANSLATION_TABLE 9

#define FALSE 0
#define TRUE 1

// mark cell
#define MARK_CELL(value) SHIFT_TO_MARKER()SET_CELL(value)RET_MARKER()

// conditional
// sets the cell 2 cells to the right to 0
#define IF_EQUAL(value, code) CODE( \
	RIGHT() \
	SET_CELL(TRUE) \
	LEFT() \
	\
	SUB(value) \
	WHILE_NZ( \
		RIGHT() \
		SET_CELL(FALSE) \
		LEFT() \
	) \
	ADD(value) \
	\
	RIGHT() \
	WHILE_NZ( \
		SET_CELL(0) \
		LEFT() \
		CODE(code) \
		RIGHT() \
	) \
	LEFT() \
)

// find marker
#define _GOTO_MARKER_L(marker) \
	SHIFT_TO_MARKER() \
		SUB(marker) \
		WHILE_NZ(ADD(marker)LEFT()SUB(marker)) \
		ADD(marker) \
	RET_MARKER()
#define _GOTO_MARKER_R(marker) \
	SHIFT_TO_MARKER() \
		SUB(marker) \
		WHILE_NZ(ADD(marker)RIGHT()SUB(marker)) \
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
	GOTO_MARKER(index_marker) \
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
		DO_OP_FACTORS(SUB, 3, 11)  /* 33 is added before hand to every input for them to be printable characters */ \
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
	RIGHT() \
	MARK_CELL(ARRAY_HEAD) \
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
		RIGHT() \
		MARK_CELL(ARRAY_HEAD) \
		\
		/* copy the new sym to the tape head cell */ \
		COPY(ARRAY_HEAD, TAPE_HEAD) \
		\
		/* go to the move cell of the action */ \
		MARK_CELL(UNMARKED) \
		RIGHT() \
		MARK_CELL(ARRAY_HEAD) \
		\
		IF_EQUAL(LEFT_VALUE,  /* RIGHT */ \
			GOTO_MARKER(TAPE_HEAD) \
			MARK_CELL(UNMARKED) \
			RIGHT() \
			MARK_CELL(TAPE_HEAD) \
			GOTO_MARKER(ARRAY_HEAD) \
		) \
		IF_EQUAL(RIGHT_VALUE,  /* LEFT */ \
			GOTO_MARKER(TAPE_HEAD) \
			\
			/* test to see if it is the tape barrier */ \
			LEFT() \
			SHIFT_TO_MARKER() \
				IF_EQUAL(TAPE_BARRIER, \
					/* this is the tape barrier! abort! */ \
					RET_MARKER() \
					/*  put the curr state to 0 so it stops */ \
					GOTO_MARKER(CURR_STATE_CELL) \
					SET_CELL(0) \
					/* put the movement to ABORT_VALUE to know the cause of the stop */ \
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
	GOTO_MARKER(COPY_CELL)  /* can't change the value of the movement cell */ \
	SET_MULTIPLE_CELLS(0, 2) \
	\
	DO_OP_FACTORS(ADD, 6, 11)  /* 66 */ \
	SUB(1)PRINT()   /* A = 65 */ \
	ADD(2)PRINT()   /* C = 67 */ \
	PRINT()         /* C = 67 */ \
	ADD(2)PRINT()   /* E = 69 */ \
	ADD(11)PRINT()  /* P = 80 */ \
	ADD(4)PRINT()   /* T = 84 */ \
	SUB(15)PRINT()  /* E = 69 */ \
	SUB(1)PRINT()   /* D = 68 */ \
	SET_CELL(0) \
	ADD(10)PRINT()  /* \n = 10 */ \
	\
	/* print the output of the machine */ \
	GOTO_MARKER(ARRAY_HEAD) \
	MARK_CELL(UNMARKED) \
	\
	GOTO_MARKER(TAPE_HEAD) \
	WHILE_NZ(  /* until you find a blank sym (0) */ \
		/* set up array search for the printable character corresponding to the sym number */ \
		GOTO_MARKER(TRANSLATION_TABLE) \
		RIGHT() \
		MARK_CELL(ARRAY_HEAD) \
		\
		ARRAY_SEARCH(TAPE_HEAD, UNMARKED) \
		\
		GOTO_MARKER(ARRAY_HEAD) \
		PRINT() \
		MARK_CELL(UNMARKED) \
		\
		/* move to tape head to the next cell */ \
		GOTO_MARKER(TAPE_HEAD) \
		MARK_CELL(UNMARKED) \
		RIGHT() \
		MARK_CELL(TAPE_HEAD) \
	) \
)

#define DO_REJECT() CODE( \
	GOTO_MARKER(COPY_CELL)  /* can't change the value of the movement cell */ \
	SET_MULTIPLE_CELLS(0, 2) \
	\
	DO_OP_FACTORS(ADD, 8, 10)  /* 80 */ \
	ADD(2)PRINT()   /* R = 82 */ \
	SUB(13)PRINT()  /* E = 69 */ \
	ADD(5)PRINT()   /* J = 74 */ \
	SUB(5)PRINT()   /* E = 69 */ \
	SUB(2)PRINT()   /* C = 67 */ \
	ADD(17)PRINT()  /* T = 84 */ \
	SUB(15)PRINT()  /* E = 69 */ \
	SUB(1)PRINT()   /* D = 68 */ \
)

#define DO_ABORT() CODE( \
	GOTO_MARKER(COPY_CELL)  /* just to be consistent */ \
	SET_MULTIPLE_CELLS(0, 2) \
	\
	DO_OP_FACTORS(ADD, 6, 11)  /* 66 */ \
	SUB(1)PRINT()   /* A = 65 */ \
	ADD(1)PRINT()   /* B = 66 */ \
	ADD(13)PRINT()  /* O = 79 */ \
	ADD(3)PRINT()   /* R = 82 */ \
	ADD(2)PRINT()   /* T = 84 */ \
	SUB(15)PRINT()  /* E = 69 */ \
	SUB(1)PRINT()   /* D = 68 */ \
)

#define OUTPUT_RESULT() CODE(\
	GOTO_MARKER(ARRAY_HEAD) \
	\
	IF_EQUAL(ACCEPT_VALUE, DO_ACCEPT()) \
	IF_EQUAL(REJECT_VALUE, DO_REJECT()) \
	IF_EQUAL(ABORT_VALUE, DO_ABORT()) \
	/* finish output */ \
	SET_CELL(0) \
	ADD(10)PRINT()  /* \n = 10 */ \
)

#define TURING_MACHINE_BF_PROGRAM CODE( \
	READ_TAPE() \
	SIMULATE() \
	OUTPUT_RESULT() \
)



// -----------------------------
// cleanup
// -----------------------------



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
