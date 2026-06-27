#/bin/bash

cpp -P turing_machine_bf.txt | sed 's/ //g' | tee turing_machine.bf
