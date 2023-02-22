#!/usr/bin/python3

from subprocess import Popen, PIPE
import csv
import os

folders = [
    ("sum",
        [
        ("Sum (Serial)", "./sum_serial", [])
        ]
    ),
    ("axpy",
        [
        ("AXPY (Serial)", "./axpy_serial", [])
        ]
    )
]

output_file = "results.csv"
max_count = 10

for folder in folders:
    os.chdir(folder[0])
    data = list()
    tests = folder[1]
    
    for test in tests:
        cmd = [test[1]] + test[2]
        row = list()
        row.append(test[0])

        for i in range(1, max_count + 1):
            process = Popen(cmd, stdout=PIPE)
            (output, err) = process.communicate()
            exit_code = process.wait()
            row.append(str(output.strip(), 'utf-8'))

        row.append("=AVERAGE(A1:A" + str(i) + ")")
        data.append(row)

    with open(output_file, "w+") as f:
        writer = csv.writer(f, delimiter=',')
        writer.writerows(data)

    os.chdir("..")