#!/usr/bin/python3

from subprocess import Popen, PIPE
import os
import os.path
import sys

def build_tests(flags1, flags2, is_arm = False):
    serial_flags = ["-O0", "-lm"]
    folders = list()
    
    # Build our configuration
    # Syntax: [file_name, output_name, compiler, args]
    for f in base_folders:
        items = list()
        
        # The serial programs
        items.append((f + "_serial.c", f + "_serial", "clang", serial_flags))
        items.append((f + "_serial.c", f + "_autovec1", "clang", flags1))
        items.append((f + "_serial.c", f + "_autovec2", "clang", flags2))
        
        # The OpenMP programs
        items.append((f + "_float.c", f + "1", "clang", flags1))
        items.append((f + "_float.c", f + "2", "clang", flags2))
        #items.append((f + "_float_unroll.c", f + "u1", "clang", flags1))
        #items.append((f + "_float_unroll.c", f + "u2", "clang", flags2))
        #items.append((f + "_float_tile.c", f + "t1", "clang", flags1))
        #items.append((f + "_float_tile.c", f + "t2", "clang", flags2))
        
        # Compile the Rose programs
        if is_arm:
            items.append(("rose_" + f + "_float.c", f + "_rex1", "clang", flags1))
            items.append(("rose_" + f + "_float.c", f + "_rex2", "clang", flags2))
            items.append(("rose_" + f + "_float_unroll.c", f + "_rex1_unroll", "clang", flags1))
            items.append(("rose_" + f + "_float_unroll.c", f + "_rex2_unroll", "clang", flags2))
            items.append(("rose_" + f + "_float_tile.c", f + "_rex1_tile", "clang", flags1))
            items.append(("rose_" + f + "_float_tile.c", f + "_rex2_tile", "clang", flags2))
        else:
            items.append(("rose_" + f + "_float.c", f + "_rex1", "clang", flags1))
            items.append(("rose_" + f + "_float.c", f + "_rex2", "clang", flags2))
            items.append(("rose_" + f + "_float_unroll.c", f + "_rex1_unroll", "clang", flags1))
            items.append(("rose_" + f + "_float_unroll.c", f + "_rex2_unroll", "clang", flags2))
            items.append(("rose_" + f + "_float_tile.c", f + "_rex1_tile", "clang", flags1))
            items.append(("rose_" + f + "_float_tile.c", f + "_rex2_tile", "clang", flags2))

        # Add
        folders.append((f, items))
    
    return folders

##
## Generates the rose programs
##
def build_rose():
    rose_flags1 = ["-rose:openmp:lowering", "-rose:skipfinalCompileStep", "-rose:simd:intel-avx"]
    rose_flags2 = ["-rose:openmp:lowering", "-rose:skipfinalCompileStep", "-rose:simd:arm-sve"]
    folders = list()
    
    # Build our configuration
    # Syntax: [file_name, output_name, compiler, args]
    for f in base_folders:
        items = list()
        
        # Build the Rose programs
        items.append((f + "_float.c", None, rose, rose_flags2))
        items.append((f + "_float_unroll.c", None, rose, rose_flags2))
        items.append((f + "_float_tile.c", None, rose, rose_flags2))
        
        items.append(("rose_" + f + "_float_sve.c", None, "mv", ["rose_" + f + "_float.c"]))
        items.append(("rose_" + f + "_float_unroll_sve.c", None, "mv", ["rose_" + f + "_float_unroll.c"]))
        items.append(("rose_" + f + "_float_tile_sve.c", None, "mv", ["rose_" + f + "_float_tile.c"]))
        
        items.append((f + "_float.c", None, rose, rose_flags1))
        items.append((f + "_float_unroll.c", None, rose, rose_flags1))
        items.append((f + "_float_tile.c", None, rose, rose_flags1))

        # Add
        folders.append((f, items))
    
    return folders

build_os = None
if len(sys.argv) == 1:
    print("Error: Please specify OS")
    exit(1)
if sys.argv[1] != "intel" and sys.argv[1] != "arm" and sys.argv[1] != "rose":
    print("Error: Only Intel, Arm, and Rose are supported.")
    exit(1)
build_os = sys.argv[1]

rose = "/home/patrick/passlab/rex_build/bin/rose-compiler"
#rose = "/home/pflynn5/passlab/rex_build/bin/rose-compiler"

base_flags = ["-O2", "-lm", "-fopenmp" ]
avx2_flags = base_flags + ["-march=native"]
avx512_flags = base_flags + ["-march=knl", "-mavx512dq"]
arm_flags1 = base_flags + ["-march=armv8-a+sve"]
arm_flags2 = base_flags + ["-march=armv8-a+sve", "-ffp-model=fast"]

base_folders = [ "axpy", "sum", "matmul", "matvec", "spmv" ]
#base_folders = ["axpy"]
folders = list()

if build_os == "intel":
    folders = build_tests(avx2_flags, avx512_flags)
elif build_os == "arm":
    folders = build_tests(arm_flags1, arm_flags2, True)
elif build_os == "rose":
    folders = build_rose()

for f in folders:
    os.chdir(f[0])
    if not os.path.exists("./build"):
        os.mkdir("build")
    programs = f[1]
    for program in programs:
        cmd = [program[2]] + program[3] + [program[0]]
        if (program[1] is not None) and (program[2] != "mv"):
            cmd += ["-o", "./build/" + program[1]]
        print(cmd)
        
        process = Popen(cmd, stdout=PIPE)
        (output, err) = process.communicate()
        exit_code = process.wait()
        print(str(output, 'utf-8'))
        if err is not None: print(str(err))
        print("")
    os.chdir("..")

