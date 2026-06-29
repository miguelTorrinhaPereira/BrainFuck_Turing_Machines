I have two simulators, one in C and another in Brain Fuck.
Both use the same interface to receive the turing machine description and input tape.

To use the bf simulator you need to get the bf code from turing_machine_bf.c (it generates it) and use `bf_tape_creator.c` to create the input for the simulator.
You can use `make bf` to generate `turing_machine.bf` (containing the bf code) and `./bf_tape_creator`.

Both simulators have S movements.
The C simulator has a bidirectional tape, the bf simulator doesn't.


Here is an example of how to use the interface of `./turing_machine` and `./bf_tape_creator` (they are the same).

```
// turing machine that determines the if a word consists of alternating 0s and 1s

alphabet: 01             // _ is the blanck symbol and is included by default
extra states count: 2    // qin, qac and qrej exist by default
qin:
        _ -> _, R, qac   // <new symbol>, <movement>, <new state>
        0 -> 0, r, q1    // the movement letter can be upper or lower case
        1 -> 1, r, q2    // movements can be L, S or R (Left, Stay, Right)
q1:
        _ -> _, R, ac    // you can omit the q
        0 ->             // if you just press enter, the action will be 1, S, qrej
        1 -> 1, R, 2
q2:
        _ -> _,R,qac     // you can omit the spaces
        0 -> 0, R, Q1    // you can also make the q upper case
        1 -> 1, S, qrej  // the default action

input: 010101
```
