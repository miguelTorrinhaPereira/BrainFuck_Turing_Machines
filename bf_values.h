#pragma once

// marker values
#define UNMARKED 0
#define COPY_CELL 1
#define CURR_STATE_CELL 2
#define CURR_SYM_CELL 3
#define STATE_START 4
#define ACTION_START 5
#define ARRAY_HEAD 6
#define TAPE_HEAD 7
#define TAPE_BARRIER 8
#define TRANSLATION_TABLE 9

// movement cell values, also includes final states
#define LEFT_VALUE 0
#define STAY_VALUE 1
#define RIGHT_VALUE 2
#define ACCEPT_VALUE 3
#define REJECT_VALUE 4
#define ABORT_VALUE 5
