#!/usr/bin/bash
# A script to run the Problem 8.18:
# - A massive number of runs are needed to test a value.
# - The ouput is not terminated by a newline and it hinders
#   repetitive runs on the command line.
#   Ref: [[file:README.org::#help-script-forkprob2]]

if [[ -z $1 ]]; then
    echo "Usage: $0 <test_value> [<iteration_n>]" 1>&2
    exit 1
fi

N=$2
[[ -z $N ]] && N=1000

for((i = 0; i < $N; i++)); do
    RC=$(./forkprob2)
    if [[ $RC -eq $1 ]]; then
	echo "Match $RC"
	exit 0
    fi
done

echo "No match! Try more iterations."
