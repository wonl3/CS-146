#!/bin/bash

gcc -std=c99 every.c -o every && ./every -10,2 asdhasodha every.c

rm -f every
