all: turing_machine bf


turing_machine:
	gcc turing_machine.c segment_array.c -o turing_machine
	

bf:
	gcc turing_machine_bf.c segment_array.c -o bf.out
	./bf.out > turing_machine.bf
	rm bf.out

	gcc bf_tape_creator.c segment_array.c -o bf_tape_creator


clean:
	rm -f bf_tape_creator turing_machine turing_machine.bf
