#/bin/bash

cpp -P turing_machine_bf.txt | python3 clean_bf.py | tee turing_machine.bf
