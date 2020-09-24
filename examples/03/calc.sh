#!/bin/bash

# Arguments
# 1 - pw.x input file

pw.x < $1 >& ${1}.out
result=`cat ${1}.out | grep "JOB DONE" | wc -l`
if [[ ${result} == 1 ]]; then
  cat ${1}.out | grep "!" | grep "total energy" | tail -n 1 | awk '{print $5}'
else
  echo Calculations failed.
fi

