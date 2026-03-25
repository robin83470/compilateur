#!/usr/bin/env python3

# In "multiple-files mode" (by default), this script runs both GCC and
# IFCC on each test-case provided and compares the results.
#
# In "single-file mode", we mimic the CLI behaviour of GCC i.e. we
# interpret the '-o', '-S', and '-c' options.
#
# Run "python3 ifcc-test.py --help" for more info.

import argparse
import glob
import os
import shutil
import sys
import subprocess

def default_target():
    machine = os.uname().machine.lower()
    if machine in ['arm64', 'aarch64']:
        return 'arm64'
    return 'x86_64'

def gcc_driver(target):
    if sys.platform == 'darwin':
        return f'gcc -arch {target}'
    return 'gcc'

def normalize_target(value):
    if value in ['x86_64', 'arm64']:
        return value
    raise argparse.ArgumentTypeError("target must be one of: x86_64, arm64")

def run_command(string, logfile=None, toscreen=False, stdin_data=None):
    """ execute `string` as a shell command. Maybe write stdout+stderr to `logfile` and/or to the toscreen.
        return the exit status"""

    if args.debug:
        print("ifcc-test.py: "+string)

    process=subprocess.Popen(string,shell=True,
                             stderr=subprocess.STDOUT,stdout=subprocess.PIPE,
                             stdin=subprocess.PIPE if stdin_data is not None else None,
                             text=True,bufsize=0)
    if logfile:
        logfile=open(logfile,'w')

    if stdin_data is not None:
        process.stdin.write(stdin_data)
        process.stdin.close()

    while True:
        output = process.stdout.readline()
        if len(output) == 0: # only happens when 'process' has terminated
            break
        if logfile: logfile.write(output)
        if toscreen: sys.stdout.write(output)
    process.wait() # collect child exit status
    assert process.returncode != None # sanity check (I was using poll() instead of wait() previously, and did see some unsanity)
    if logfile:
        logfile.write(f'\nexit status: {process.returncode}\n')
    return process.returncode

def dumpfile(name,quiet=False):
    data=open(name,"rb").read().decode('utf-8',errors='ignore')
    if not quiet:
        print(data,end='')
    return data

######################################################################################
## ARGPARSE step: make sense of our command-line arguments

# This is where we decide between multiple-files
# mode and single-file mode

import textwrap
import shutil
width = shutil.get_terminal_size().columns-2
twf=lambda text: textwrap.fill(text,width,initial_indent=' '*4,subsequent_indent=' '*6)

argparser   = argparse.ArgumentParser(
formatter_class=argparse.RawDescriptionHelpFormatter,
description = "Testing script for the ifcc compiler. operates in one of two modes:\n\n"
    +twf("- Multiple-files mode (by default): Compile several programs with both GCC and IFCC, run them, and compare the results.",)+"\n\n"
    +twf("- Single-file mode (with options -o,-c and/or -S): Compile and/or assemble and/or link a single program."),
epilog="examples:\n\n"
    +twf("python3 ifcc-test.py testfiles")+'\n'
    +twf("python3 ifcc-test.py --target arm64 testfiles")+'\n'
    +twf("python3 ifcc-test.py path/to/some/dir/*.c path/to/some/other/dir")+'\n'
    +'\n'
    +twf("python3 ifcc-test.py -o ./myprog path/to/some/source.c")+'\n'
    +twf("python3 ifcc-test.py -S -o truc.s truc.c")+'\n'
    ,
)

argparser.add_argument('input',metavar='PATH',nargs='+',help='For each path given:'
                       +' if it\'s a file, use this file;'
                       +' if it\'s a directory, use all *.c files under this subtree')

argparser.add_argument('-v','--verbose',action="count",default=0,
                       help='increase verbosity level. You can use this option multiple times.')
argparser.add_argument('-d','--debug',action="count",default=0,
                       help='increase quantity of debugging messages (only useful to debug the test script itself)')
argparser.add_argument('-S',action = "store_true", help='single-file mode: compile from C to assembly, but do not assemble')
argparser.add_argument('-c',action = "store_true", help='single-file mode: compile/assemble to machine code, but do not link')
argparser.add_argument('-o','--output',metavar = 'OUTPUTNAME', help='single-file mode: write output to that file')
default_target_value = default_target()
argparser.add_argument('--target', type=normalize_target, default=default_target_value,
                       help=f'target architecture passed to ifcc (x86_64 or arm64). Default: host architecture ({default_target_value})')

args=argparser.parse_args()

if args.debug >=2:
    print('debug: command-line arguments '+str(args))

orig_cwd=os.getcwd()
if "ifcc-test-output" in orig_cwd:
    print('error: cannot run ifcc-test.py from within its own output directory')
    exit(1)

pld_base_dir=os.path.normpath(os.path.dirname(__file__))
if args.debug:
    print("ifcc-test.py: "+os.path.dirname(__file__))

ifcc_cmd=f'{pld_base_dir}/compiler/ifcc --target {args.target}'
gcc_cmd=gcc_driver(args.target)

# cleanup stale output directory
if os.path.isdir(f'{pld_base_dir}/ifcc-test-output'):
    subprocess.run(['rm', '-rf', f'{pld_base_dir}/ifcc-test-output'], check=False)

# Ensure that the `ifcc` executable itself is up-to-date
makestatus=run_command(f'cd {pld_base_dir}/compiler; make --question ifcc')
if makestatus: # updates are needed
    makestatus=run_command(f'cd {pld_base_dir}/compiler; make ifcc',toscreen=True) # this time we run `make` for real
    if makestatus: # if `make` failed, we fail too
        if os.path.exists("ifcc"): # and we remove any out-of-date compiler (to reduce chance of confusion)
            os.unlink("ifcc")
        exit(makestatus)

##########################################
## single-file mode aka "let's act just like GCC (almost)"

if args.S or args.c or args.output:
    if args.S and args.c:
        print("error: options -S and -c are not compatible")
        exit(1)
    if len(args.input)>1:
        print("error: this mode only supports a single input file")
        exit(1)
    inputfilename=args.input[0]

    if inputfilename[-2:] != ".c":
        print("error: incorrect filename suffix (should be '.c'): "+inputfilename)
        exit(1)

    try:
        open(inputfilename,"r").close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        exit(1)

    if (args.S or args.c) and not args.output:
        print("error: option '-o filename' is required in this mode")
        exit(1)

    if args.S: # produce assembly
        if args.output[-2:] != ".s":
            print("error: output file name must end with '.s'")
            exit(1)
        ifccstatus=run_command(f'{ifcc_cmd} {inputfilename} > {args.output}')
        if ifccstatus: # let's show error messages on screen
            exit(run_command(f'{ifcc_cmd} {inputfilename}',toscreen=True))
        else:
            exit(0)

    elif args.c: # produce machine code
        if args.output[-2:] != ".o":
            print("error: output file name must end with '.o'")
            exit(1)
        asmname=args.output[:-2]+".s"
        ifccstatus=run_command(f'{ifcc_cmd} {inputfilename} > {asmname}')
        if ifccstatus: # let's show error messages on screen
            exit(run_command(f'{ifcc_cmd} {inputfilename}',toscreen=True))
        exit(run_command(f'{gcc_cmd} -c -o {args.output} {asmname}',toscreen=True))

    else: # produce an executable
        if args.output[-2:] in [".o",".c",".s"]:
            print("error: incorrect name for an executable: "+args.output)
            exit(1)
        asmname=args.output+".s"
        ifccstatus=run_command(f'{ifcc_cmd} {inputfilename} > {asmname}')
        if ifccstatus:
            exit(run_command(f'{ifcc_cmd} {inputfilename}', toscreen=True))
        exit(run_command(f'{gcc_cmd} -o {args.output} {asmname}'))

    # we should never end up here
    print("unexpected error. please report this bug.")
    exit(1)

# if we were not in single-file mode, then it means we are in
# multiple-files mode.

######################################################################################
## PREPARE step: find and copy all test-cases under ifcc-test-output

## Process each cli argument as a filename or subtree
os.chdir(orig_cwd)
inputfilenames=[]
for path in args.input:
    path=os.path.normpath(path) # collapse redundant slashes etc.
    if os.path.isfile(path):
        if path[-2:] == '.c':
            inputfilenames.append(path)
        else:
            print("error: incorrect filename suffix (should be '.c'): "+path)
            exit(1)
    elif os.path.isdir(path):
        for dirpath,dirnames,filenames in os.walk(path):
            inputfilenames+=[dirpath+'/'+name for name in filenames if name[-2:]=='.c']
    else:
        print("error: cannot read input path `"+path+"'")
        sys.exit(1)

inputfilenames=sorted(inputfilenames)
## debug: after treewalk
if args.debug:
    print("debug: list of files after tree walk:"," ".join(inputfilenames))

## sanity check
if len(inputfilenames) == 0:
    print("error: found no test-case in: "+" ".join(args.input))
    sys.exit(1)

## Check that we actually can read these files. Our goal is to
## fail as early as possible when the CLI arguments are wrong.
for inputfilename in inputfilenames:
    try:
        f=open(inputfilename,"r")
        f.close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        exit(1)

## We're going to copy every test-case in its own subdir of ifcc-test-output
os.mkdir(pld_base_dir+'/ifcc-test-output')

jobs=[]

for inputfilename in inputfilenames:
    if args.debug>=2:
        print("debug: PREPARING "+inputfilename)

    if 'ifcc-test-output' in os.path.realpath(inputfilename):
        print('error: input filename is within output directory: '+inputfilename)
        exit(1)

    ## each test-case gets copied and processed in its own subdirectory:
    ## ../somedir/subdir/file.c becomes ifcc-test-output/--somedir-subdir-file/input.c
    subdirname=inputfilename[:-2] # remove the '.c' suffix
    if pld_base_dir in subdirname: # hide "absolute" part of path when not meaningful
        subdirname=subdirname[len(pld_base_dir):]
    subdirname=subdirname.replace('..','-') # keep some punctuation to discern "bla.c" from "../bla.c"
    subdirname=subdirname.replace('./','')  # remove meaningless part of name
    subdirname=subdirname.replace('/','-')  # flatten path to single subdir
    if args.debug>=2:
        print("debug: subdir="+subdirname)

    os.mkdir(pld_base_dir+'/ifcc-test-output/'+subdirname)
    shutil.copyfile(inputfilename,pld_base_dir+'/ifcc-test-output/'+subdirname+'/input.c')
    stdin_filename=inputfilename[:-2]+'.stdin'
    if os.path.isfile(stdin_filename):
        shutil.copyfile(stdin_filename,pld_base_dir+'/ifcc-test-output/'+subdirname+'/input.stdin')
    jobs.append( (subdirname, inputfilename) )

## eliminate duplicate paths from the 'jobs' list
unique_jobs=[]
for j_tuple in jobs:
    j = j_tuple[0]
    for d_tuple in unique_jobs:
        d = d_tuple[0]
        if os.path.samefile(pld_base_dir+'/ifcc-test-output/'+j,pld_base_dir+'/ifcc-test-output/'+d):
            break # and skip the 'else' branch
    else:
        unique_jobs.append(j_tuple)
jobs=unique_jobs

# debug: after deduplication
if args.debug:
    print("debug: list of test-cases after PREPARE step:"," ".join([j[0] for j in jobs]))

######################################################################################
## TEST step: actually compile/link/run each test-case with both compilers.
##
##            if both toolchains agree, this test-case is passed.
##            otherwise, this is a fail.

all_ok=True
new_results = []
target = args.target

for jobname, orig_filepath in jobs:
    os.chdir(f'{pld_base_dir}/ifcc-test-output')

    print('TEST-CASE: '+jobname)
    os.chdir(jobname)

    test_name = os.path.basename(orig_filepath)[:-2] # remove .c

    # Group is defined by the directory immediately after testfiles/
    path_parts = os.path.normpath(orig_filepath).split(os.sep)
    group = "Unknown"
    if 'testfiles' in path_parts:
        tf_idx = path_parts.index('testfiles')
        group_dir = path_parts[tf_idx + 1] if tf_idx + 1 < len(path_parts) else ""
        if group_dir == 'InvalidPrograms':
            group = 'Invalid'
        elif group_dir == 'ValidPrograms':
            group = 'Valid'
        elif group_dir == 'NotImplementedYet':
            group = 'NotImplementedYet'

    loop_gcc_cmd = gcc_driver(target)
    loop_ifcc_cmd = f'{pld_base_dir}/compiler/ifcc --target {target}'

    ## Reference compiler = GCC
    gccstatus=run_command(f"{loop_gcc_cmd} -S -o asm-gcc-{target}.s input.c", f"gcc-compile-{target}.txt")
    if gccstatus == 0:
        # test-case is a valid program. we should run it
        gccstatus=run_command(f"{loop_gcc_cmd} -o exe-gcc-{target} asm-gcc-{target}.s", f"gcc-link-{target}.txt")
    if gccstatus == 0: # then both compile and link stage went well
        stdin_data=open("input.stdin").read() if os.path.exists("input.stdin") else None
        exegccstatus=run_command(f"./exe-gcc-{target}", f"gcc-execute-{target}.txt", stdin_data=stdin_data)
        if args.verbose >=2:
            dumpfile(f"gcc-execute-{target}.txt")

    ## IFCC compiler
    ifccstatus=run_command(f'{loop_ifcc_cmd} input.c > asm-ifcc-{target}.s', f'ifcc-compile-{target}.txt')

    status_msg = "OK"
    if gccstatus != 0 and ifccstatus != 0:
        status_msg = "OK"
    elif gccstatus != 0 and ifccstatus == 0:
        status_msg = "FAIL (your compiler accepts an invalid program)"
        all_ok=False
    elif gccstatus == 0 and ifccstatus != 0:
        status_msg = "FAIL (your compiler rejects a valid program)"
        all_ok=False
        if args.verbose:
            dumpfile(f"asm-ifcc-{target}.s", quiet=True)       
            dumpfile(f"ifcc-compile-{target}.txt", quiet=True) 
    else:
        ldstatus=run_command(f"{loop_gcc_cmd} -o exe-ifcc-{target} asm-ifcc-{target}.s", f"ifcc-link-{target}.txt")
        if ldstatus:
            status_msg = "FAIL (your compiler produces incorrect assembly)"
            all_ok=False
            if args.verbose:
                dumpfile(f"asm-ifcc-{target}.s", quiet=True)
                dumpfile(f"ifcc-link-{target}.txt", quiet=True)
        else:
            stdin_data=open("input.stdin").read() if os.path.exists("input.stdin") else None
            run_command(f"./exe-ifcc-{target}", f"ifcc-execute-{target}.txt", stdin_data=stdin_data)
            
            # Check for output differences or segfaults/crashes
            if not os.path.exists(f"gcc-execute-{target}.txt") or not os.path.exists(f"ifcc-execute-{target}.txt") or open(f"gcc-execute-{target}.txt").read() != open(f"ifcc-execute-{target}.txt").read() :
                status_msg = "FAIL (different results at execution)"
                all_ok=False

    print(f"TEST {status_msg}")
    new_results.append((test_name, group, status_msg))

# Output CSV
import csv
import sys
from datetime import datetime

csv_filename = os.path.join(orig_cwd, "test_results.csv")
headers = ["Test Name", "Group", "ARM64 Result", "x86_64 Result", "Last Run", "Comment"]
existing_data = {}

if os.path.exists(csv_filename):
    with open(csv_filename, 'r', newline='', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            if "Test Name" in row:
                existing_data[row["Test Name"]] = row

current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
target_col = "ARM64 Result" if target == "arm64" else "x86_64 Result"

for test_name, group, status_msg in new_results:
    if test_name not in existing_data:
        existing_data[test_name] = {h: "" for h in headers}
        existing_data[test_name]["Test Name"] = test_name
    existing_data[test_name]["Group"] = group
    
    existing_data[test_name][target_col] = status_msg
    existing_data[test_name]["Last Run"] = current_time

with open(csv_filename, 'w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=headers)
    writer.writeheader()
    # Write sorted by Test Name
    for test_name in sorted(existing_data.keys()):
        row = existing_data[test_name]
        writer.writerow({h: row.get(h, "") for h in headers})

print(f"Results written to {csv_filename}")

if not (all_ok or args.verbose):
    print("Some test-cases failed. Run ifcc-test.py with option '--verbose' for more detailed feedback.", file=sys.stderr)

if all_ok:
    sys.exit(0)
else:
    sys.exit(1)
