#!/bin/bash


# Running Part 2
gcc -std=c99 every.c -o every # && ./every -10,2 asdhasodha every.c

# Running Part 3
gcc -std=c99 srm.c -o srm
gcc -std=c99 unrm.c -o unrm
gcc -std=c99 trash.c -o trash

rm -f every
