#!/usr/bin/python3

from subprocess import Popen, PIPE
import csv
import os
import sys

#base_folders = [ "axpy", "sum", "matmul", "matvec", "spmv" ]
base_folders = ["matmul", "matvec", "spmv"]
folders = list()

for folder in base_folders:
    items = list()
    items.append((folder + " (Serial)",               "./build/" + folder + "_serial", []))
    items.append((folder + " (Autovec (AVX-2))",      "./build/" + folder + "_autovec1", []))
    items.append((folder + " (Autovec (AVX-512))",    "./build/" + folder + "_autovec2", []))
    items.append((folder + " (OpenMP (AVX-2))",       "./build/" + folder + "1", []))
    items.append((folder + " (OpenMP (AVX-512))",     "./build/" + folder + "2", []))
    #items.append((folder + " (OpenMP w/Unroll (AVX-2))", "./build/" + folder + "u1", []))
    #items.append((folder + " (OpenMP w/Unroll (AVX-512))", "./build/" + folder + "u2", []))
    #items.append((folder + " (OpenMP w/Tile (AVX-2))", "./build/" + folder + "t1", []))
    #items.append((folder + " (OpenMP w/Tile (AVX-512))", "./build/" + folder + "t2", []))
    items.append((folder + " (Rex (AVX-2))",        "./build/" + folder + "_rex1", []))
    items.append((folder + " (Rex (AVX-512))",        "./build/" + folder + "_rex2", []))
    items.append((folder + " (Rex w/Unroll (AVX-2))", "./build/" + folder + "_rex1_unroll", []))
    items.append((folder + " (Rex w/Unroll (AVX-512))", "./build/" + folder + "_rex2_unroll", []))
    items.append((folder + " (Rex w/Tile (AVX-2))", "./build/" + folder + "_rex1_tile", []))
    items.append((folder + " (Rex w/Tile (AVX-512))", "./build/" + folder + "_rex2_tile", []))
    folders.append((folder, items))


output_file = "results.csv"
if len(sys.argv) > 1:
    output_file = "results_" + str(sys.argv[1]) + ".csv"
max_count = 4
#max_count = 2

for folder in folders:
    os.chdir(folder[0])
    data = list()
    tests = folder[1]
    
    for test in tests:
        cmd = [test[1]] + test[2]
        row = list()
        row.append(test[0])

        for i in range(1, max_count + 1):
            try:
                process = Popen(cmd, stdout=PIPE)
                (output, err) = process.communicate()
                exit_code = process.wait()
                row.append(str(output.strip(), 'utf-8'))
            except Exception:
                continue

        row.append("=AVERAGE(A1:A" + str(i) + ")")
        data.append(row)

    with open(output_file, "w+") as f:
        writer = csv.writer(f, delimiter=',')
        writer.writerows(data)

    os.chdir("..")
