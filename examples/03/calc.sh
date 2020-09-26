#!/bin/bash

# Arguments
# 1 - pw.x input file

error_msg="Calculations failed."

pw.x < $1 >& ${1}.out

stop=`cat ${1}.out | grep "STOP" | wc -l`
if [ ${stop} -gt 0 ]; then
  echo ${error_msg}
  exit
fi

job_done=`cat ${1}.out | grep "JOB DONE" | wc -l`
if [[ ${job_done} == 1 ]]; then
  cat ${1}.out | grep "!" | grep "total energy" | tail -n 1 | awk '{print $5}'
else
  echo ${error_msg}
fi
exit

