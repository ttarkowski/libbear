#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please provide directory name:"
    echo "  $0 dir"
    exit
fi

if [ ! -d "$1" ]; then
    echo "Directory $0 does not exist."
    exit
fi

pushd $1
cp ../Makefile .
make all && make run
make clean && rm -f Makefile
popd

exit
