#!/usr/bin/env python3

import sys
import os

if (len(sys.argv) < 1):
    sys.exit(-1)
if (len(sys.argv) != 2):
    print("Usage:", sys.argv[0], "[Directory]")
    sys.exit(-1)

dir = sys.argv[1]

if (dir[len(dir) - 1] != '/'):
    dir += "/"
output = dir + "pt_dump"
files = os.listdir(dir)
bigsize = 0
bignum = 0
input = ""

for eachfile in files:
    currpath = dir + eachfile
    if (os.path.isfile(currpath) and eachfile.startswith("pt_dump_raw.")):
        currsize = os.path.getsize(currpath)
        filename_components = eachfile.split(".")
        if (len(filename_components) != 2):
            print("WARNING: Illegal pt_dump_raw file:", currpath)
            continue
        currnum = filename_components[1]
        if (("pmap_raw." + currnum) not in files):
            print("WARNING: pmap_raw." + currnum, "missing")
            continue
        if (os.path.getsize(dir + "pmap_raw." + currnum) == 0):
            print("WARNING: pmap_raw." + currnum, "is empty")
            continue
        
        if (currsize > bigsize):
            bigsize = currsize
            bignum = int(currnum)
            input = currpath
        elif (currsize == bigsize and int(currnum) > bignum):
            bignum = int(currnum)
            input = currpath

if (input == ""):
    print("Check directory (failed to match pt_dump_raw and pt_dump_raw files)")
    sys.exit(-1)

print("Picked input:", input, "(" + str(bigsize) + ") bytes")

os.rename(input, output)
