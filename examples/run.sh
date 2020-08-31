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
make all
if ([[ "$?" == "0" ]])
then
    echo Compilation successful.
    make run
    if ([[ "$?" == "0" ]])
    then
        echo Successful program execution.
    else
        echo Program has failed.
        make check
    fi
else
    echo Compilation has failed.
fi
make clean && rm -f Makefile
popd

exit
