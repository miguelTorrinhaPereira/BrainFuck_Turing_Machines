#pragma once

// marker values
#define UNMARKED 0
#define COPY_CELL 1
#define CONDITION_CELL 2
#define CURR_STATE_CELL 3
#define CURR_SYM_CELL 4
#define STATE_START 5
#define ACTION_START 6
#define ARRAY_HEAD 7
#define TRANSLATION_TABLE 8
#define TAPE_BARRIER 9
#define TAPE_HEAD 10
#define FINAL_STATUS 11

// movement cell values, also includes final states
#define LEFT_VALUE 0
#define STAY_VALUE 1
#define RIGHT_VALUE 2
#define ACCEPT_VALUE 3
#define REJECT_VALUE 4
#define ABORT_VALUE 5

// sumed to make the characters printable
#define BF_INPUT_OFFSET 34

#define TERMINAL_STATE 0
