I have two simulators, one in C and another in Brain Fuck.
Both use the same interface to receive the turing machine description and input tape.

To use the bf simulator you need to get the bf code from turing_machine_bf.c (it generates it) and use `bf_tape_creator.c` to create the input for the simulator.
You can use `make bf` to generate `turing_machine.bf` (containing the bf code) and `./bf_tape_creator`.

You can use the `-h` option with `./turing_machine` and `./bf_tape_creator` to see how to use the interface.
