#!/bin/bash

# Arguments
# 1 - pseudopotential file

if ! [ -f "$1" ]; then
  wget https://www.quantum-espresso.org/upf_files/$1 >& /dev/null
fi

