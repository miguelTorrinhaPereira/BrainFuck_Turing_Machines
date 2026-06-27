// high level cell = [ Marker | Data ]
#define CELL_SIZE 2
// [copy cell][curr symbol][curr state](states: (state: [state_header](actions: (action: [new state][move]), ...)), ...)[tape barrier](tape: [tape cell], ...)

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
#define ADD(value) REPEAT(value, +)
#define SUB(value) REPEAT(value, -)
#define INC() ADD(1)
#define DEC() SUB(1)

// set current cell
#define _SET_ZERO() [-]
#define SET(value) _SET_ZERO()ADD(value)

// [ Marker | Data ]
#define SHIFT_TO_MARKER() <
#define RET_MARKER() >

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

// movement
#define LEFT(count) REPEAT(count, REPEAT(CELL_SIZE, <))
#define RIGHT(count) REPEAT(count, REPEAT(CELL_SIZE, >))

// conditional
#define WHILE_NZ(code) [CODE(code)]

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
#define GOTO_MARKER(marker) _GOTO_MARKER_L(COPY_CELL)_GOTO_MARKER_R(marker)

// data movement
// doesn't celan the dest cell
// assumes it is starting in the orig cell
#define MOVE(orig_marker, dest_marker) \
	WHILE_NZ( \
		GOTO_MARKER(dest_marker)INC() \
		GOTO_MARKER(orig_marker)DEC() \
#define COPY(orig_marker, dest_marker) \
	MOVE(orig_marker, COPY_CELL) \
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

// main function
GOTO_MARKER(CURR_STATE_CELL) \
WHILE_NZ( \
	// setup array search to get the curr state
	// go to the first cell of the first state
	GOTO_MARKER(STATE_START) \
	MARK_CELL(ARRAY_HEAD) \
	// go back to the curr state cell
	GOTO_MARKER(CURR_STATE_CELL) \
	// the index cell end with the value 0
	ARRAY_SEARCH(CURR_STATE_CELL, STATE_START) \

	// copy the curr symbol of the turing machine
	GOTO_MARKER(TAPE_HEAD) \
	COPY(TAPE_HEAD, CURR_SYM_CELL) \
	GOTO_MARKER(CURR_SYM_CELL) \

	// get the correct action, we are already have marked the correct state
	GOTO_MARKER(ARRAY_HEAD) \
	UNMARK_CELL()
	// skip state header
	RIGHT(1) \
	MARK_CELL(ARRAY_HEAD) \
	GOTO_MARKER(CURR_SYM_CELL) \
	ARRAY_SEARCH(CURR_SYM_CELL, ACTION_START) \

	// copy the next state to curr state cell
	GOTO_MARKER(ARRAY_HEAD) \
	COPY(ARRAY_HEAD, CURR_STATE_CELL) \

	UNMARK_CELL() \
	// go to the move cell of the action
	RIGHT(1) \
	MARK_CELL(ARRAY_HEAD) \

	// move is 1 for right, 0 for left
	WHILE_NZ( \
		// we need to move it to the right, so move the head to the right 2 times
		// latter it will be moved it to the left one time
		GOTO_MARKER(TAPE_HEAD) \
		UNMARK_CELL() \
		RIGHT(2) \
		MARK_CELL(TAPE_HEAD) \
		GOTO_MARKER(ARRAY_HEAD) \
	) \
	GOTO_MARKER(TAPE_HEAD) \

	// test to see if it is the tape barrier
	// TRUE
	MARK_CELL(1) \
	LEFT(1) \
	SHIFT_TO_MARKER() \
		SUB(TAPE_BARRIER) \
		WHILE_NZ( \
			// it isn't the tape barrier
			RIGHT(1) \
			// FALSE
			MARK_CELL(0) \
			LEFT(1) \
		) \
		ADD(TAPE_BARRIER) \
		RIGHT(1) \
		WHILE_NZ( \
			// this is the tape barrier! put the curr state to 0
			RET_MARKER() \
			GOTO_MARKER(CURR_STATE_CELL) \
			SET(0) \
			GOTO_MARKER(TAPE_BARRIER) \
			RIGHT(1) \
			SHIFT_TO_MARKER() \
		) \
		// added for readability
		LEFT(1) \
	RET_MARKER() \
	RIGHT(1) \
	UNMARK_CELL() \
	// actually do the left movement
	LEFT(1) \
	MARK_CELL(TAPE_HEAD) \
	// if the state is 0, the machine stops
	GOTO_MARKER(CURR_STATE_CELL) \
)
