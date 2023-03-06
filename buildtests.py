#!/usr/bin/python3

from subprocess import Popen, PIPE
import os
import os.path

#rose = "/home/patrick/passlab/build/bin/rose-compiler"
#rose = "/opt/rex_install/bin/rose-compiler"
rose = "/home/pflynn5/passlab/rex_build/bin/rose-compiler"

serial_flags = ["-O0", "-lm"]
avx2_flags = ["-O2", "-lm", "-march=native"]
avx512_flags = ["-O2", "-lm", "-march=knl", "-mavx512dq"]
rose_flags1 = ["-rose:openmp:lowering", "-rose:skipfinalCompileStep", "-rose:simd:intel-avx"]

base_folders = [ "axpy", "sum", "matmul", "matvec", "spmv" ]
folders = list()

# Build our configuration
# Syntax: [file_name, output_name, compiler, args]
for f in base_folders:
    items = list()
    
    # The serial programs
    items.append((f + "_serial.c", f + "_serial", "clang", serial_flags))
    items.append((f + "_serial.c", f + "_autovec1", "clang", avx2_flags))
    items.append((f + "_serial.c", f + "_autovec2", "clang", avx512_flags))
    
    # The OpenMP programs
    items.append((f + "_float.c", f + "1", "clang", avx2_flags))
    items.append((f + "_float.c", f + "2", "clang", avx512_flags))
    items.append((f + "_float_unroll.c", f + "u1", "clang", avx2_flags))
    items.append((f + "_float_unroll.c", f + "u2", "clang", avx512_flags))
    
    # Build the Rose programs
    items.append((f + "_float.c", None, rose, rose_flags1))
    items.append((f + "_float_unroll.c", None, rose, rose_flags1))
    
    # Compile the Rose programs
    items.append(("rose_" + f + "_float.c", f + "_rex_avx2", "clang", avx2_flags))
    items.append(("rose_" + f + "_float.c", f + "_rex_avx512", "clang", avx512_flags))
    items.append(("rose_" + f + "_float_unroll.c", f + "_rex_avx2_unroll", "clang", avx2_flags))
    items.append(("rose_" + f + "_float_unroll.c", f + "_rex_avx512_unroll", "clang", avx512_flags))

    # Add
    folders.append((f, items))


for f in folders:
    os.chdir(f[0])
    if not os.path.exists("./build"):
        os.mkdir("build")
    programs = f[1]
    for program in programs:
        print(os.getcwd())
        cmd = [program[2]] + program[3] + [program[0]]
        if program[1] is not None:
            cmd += ["-o", "./build/" + program[1]]
        print(cmd)
        
        process = Popen(cmd, stdout=PIPE)
        (output, err) = process.communicate()
        exit_code = process.wait()
        print(str(output, 'utf-8'))
        if err is not None: print(str(err))
        print("")
    os.chdir("..")

