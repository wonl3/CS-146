#!/bin/sh

# a string to print when we are called incorrectly.
USAGE="USAGE: every [-N,M] [<list-of-files>], where M <= N"

# Check the environment for default parameters.
case "$EVERY" in
"") EVERY=-1,1
    ;;
*)  # EVERY is set to something.  Revise the usage message to remind people
    # to check the evirnoment variable for correct syntax.
    USAGE="$USAGE (check env var EVERY)"
    ;;
esac

case "$1" in
-*) EVERY=$1
    # check syntax of $EVERY
    if echo "$EVERY" | egrep '^-[1-9][0-9]*($|,[1-9][0-9]*$)' >/dev/null; then
	: # OK
    else
	echo "$USAGE" >&2
	exit 1
    fi

    # We now know $EVERY has the correct form, -N,M, N,M both positive integers.
    shift
    ;;
esac

# This is really inefficient if we got the values from $EVERY, or if
# they are the default of -1,1.  We really should find a way to do it
# without having to parse this mess unless we are passed options on
# the command line.
every=`echo $EVERY | sed 's/^-\([0-9]*\).*$/\1/'`
print=`echo $EVERY | sed 's/^.*,\([0-9]*\)$/\1/'`
# if the second match didn't work, there was no comma and $print is -something.
case "$print" in
    -*) print=1 ;;
esac

if [ "$print" -gt "$every" ]; then  
    echo "$USAGE" >&2
    exit 1
fi

# if we just did awk ... "$@", awk treats the entire list of files as
# a single stream, so NR wouldn't get reset at the beginning of each.
# Thus, we need this mess to simulate resetting NR on each file, and
# handling the special case of $#=0.  (Because if we just did a loop
# on $* and $#=0, then the loop doesn't get executed at all.)

case $# in
0)  awk '(NR-1) % '$every' < '$print' { print }' ;;
*)  for i   # handle each file independently.
    do
	awk '(NR-1) % '$every' < '$print' { print }' "$i"
    done
esac
