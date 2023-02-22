#!/usr/bin/python3

from subprocess import Popen, PIPE

# Syntax: [file_name, output_name,]
programs = [
    ("program.cpp", "program1", "g++", ["-O0"]),
    ("program.cpp", "program2", "g++", ["-O1"]),
]

for program in programs:
    cmd = [program[2]] + program[3] + [program[0]] + ["-o", program[1]]
    print(cmd)
    
    process = Popen(cmd, stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    print(str(output, 'utf-8'))
    if err is not None: print(str(err))
    print("")