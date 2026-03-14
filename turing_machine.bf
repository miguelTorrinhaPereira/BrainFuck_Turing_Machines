// used to avoid spaces
#define CODE(code) code

// increments
#define ADD_0
#define ADD_1  +
#define ADD_2  ++
#define ADD_3  +++
#define ADD_4  ++++
#define ADD_5  +++++
#define ADD_6  ++++++
#define ADD_7  +++++++
#define ADD_8  ++++++++
#define ADD_9  +++++++++
#define ADD_10 ++++++++++
#define ADD(value) ADD_##value

// decrements
#define SUB_0
#define SUB_1  -
#define SUB_2  --
#define SUB_3  ---
#define SUB_4  ----
#define SUB_5  -----
#define SUB_6  ------
#define SUB_7  -------
#define SUB_8  --------
#define SUB_9  ---------
#define SUB_10 ----------
#define SUB(value) SUB_##value

// set curr cell
#define SET_Z() [-]
#define SET(value) SET_Z()ADD(value)

// [ Marker | Conditional | Backup | Data ]
#define GO_M() <<<
#define GO_C() <<
#define GO_B() <

#define RET_M() >>>
#define RET_C() >>
#define RET_B() >

#define SET_M(value) GO_M()SET(value)RET_M()
#define SET_C(value) GO_C()SET(value)RET_C()
#define SET_B(value) GO_B()SET(value)RET_B()

// conditional, no nested ifs in the same cell
#define IF_NZ(code) GO_C()SET(0)[RET_C()CODE(code)GO_C()]RET_C()
//FIXME: make this functional
#define IF_Z(code) SET_C(1)[GO_C()SET(0)]GO_C()[RET_C()CODE(code)GO_C()]RET_C()
#define IF(value, code) \
	SUB(value)IF_Z(\
		ADD(value)CODE(code)SUB(value)\
	)ADD(value)

// special control values
#define MARCK 0
#define DATA 1
#define HEAD_TAPE 2
#define HEAD_STATE 3
#define STATES_START 4
#define STATE_START 5
#define ACTION_START 6

// movement
#define LEFT_0
#define LEFT_1  <<<<
#define LEFT_2  <<<<<<<<
#define LEFT_3  <<<<<<<<<<<<
#define LEFT_4  <<<<<<<<<<<<<<<<
#define LEFT_5  <<<<<<<<<<<<<<<<<<<<
#define LEFT_6  <<<<<<<<<<<<<<<<<<<<<<<<
#define LEFT_7  <<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define LEFT_8  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define LEFT_9  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define LEFT_10 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#define RIGHT_0
#define RIGHT_1  >>>>
#define RIGHT_2  >>>>>>>>
#define RIGHT_3  >>>>>>>>>>>>
#define RIGHT_4  >>>>>>>>>>>>>>>>
#define RIGHT_5  >>>>>>>>>>>>>>>>>>>>
#define RIGHT_6  >>>>>>>>>>>>>>>>>>>>>>>>
#define RIGHT_7  >>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define RIGHT_8  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define RIGHT_9  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define RIGHT_10 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


