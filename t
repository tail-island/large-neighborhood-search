#!/bin/sh

for path in $(find $(dirname $0)/data -type f -name \*.txt | sort);
do
    echo ${path}

    cat ${path} | ./build/large-neighborhood-search | python extra/visualize_solution.py ${path}
done
