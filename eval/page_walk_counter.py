#!/usr/bin/env python3

import sys
import os

if (len(sys.argv) < 1):
    sys.exit(-1)
if (len(sys.argv) != 2):
    print("Usage:", sys.argv[0], "[perf.log]")
    sys.exit(-1)

#print("Loading")
input = sys.argv[1]
file = []
with open(input) as infile:
    for line in infile:
        file.append(line)
#print("Done")

valid_cols = ["dtlb_load_misses.walk_completed", "dtlb_load_misses.walk_pending", "dtlb_load_misses.walk_active",
              "dtlb_store_misses.walk_completed", "dtlb_store_misses.walk_pending", "dtlb_store_misses.walk_active",
              "itlb_misses.walk_completed", "itlb_misses.walk_pending", "itlb_misses.walk_active",
              "cycles:ukhHG"]

# Read a line in the perf file
# If the line is the signalling message -- return the raw message (len = 1)
# If the line contains valid data, return the 3 columns of interest (len = 3)
# If the line is invalid, return an empty list (len = 0)
def getcols(line):
    time = 0.0
    counts = 0
    col = 0
    components = line.split(" ")
    if components[0] == "signalling":
        return [line]
    for component in components:
        if component == "":
            continue
        try:
            if col == 0:
                time = float(component)
                col += 1
            elif col == 1:
                component = component.replace(",", "")
                counts = int(component)
                col += 1
            elif col == 2:
                if component in valid_cols:
                    return [time, counts, component]
                else:
                    return []
        except:
            return []
    return []

starttime = 0.0
endtime = 0.0
eventcounts = [0] * len(valid_cols)
lnread = 0

for i in range(len(file) - 1, -1, -1):
    cols = getcols(file[i])
    if len(cols) == 3:
        if cols[0] > endtime:
            endtime = cols[0]
        #print(cols)
        eventcounts[valid_cols.index(cols[2])] += cols[1]
        starttime = cols[0]  # Hey the first column must be decreasing right?
    elif len(cols) == 1:
        i_overrun = i - 1
        cols_overrun = getcols(file[i_overrun])
        lnread += 1
        while len(cols_overrun) == 0 or len(cols_overrun) == 1 or (len(cols_overrun) == 3 and cols_overrun[0] == starttime):
            if len(cols_overrun) == 3:
                eventcounts[valid_cols.index(cols_overrun[2])] += cols_overrun[1]
            i_overrun -= 1
            cols_overrun = getcols(file[i_overrun])
            lnread += 1
        break
    lnread += 1

# print("Read", lnread, "lines")
print("duration\t", endtime - starttime)
for i in range(len(valid_cols)):
    print(valid_cols[i], eventcounts[i], sep="\t")
