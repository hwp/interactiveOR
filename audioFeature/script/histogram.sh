#!/bin/sh

while read line
do
  cl=${line%% *}
  seq=${line#*[A-Z] }
  echo -n "$cl "
  tot=`echo $seq | wc -w`
  echo $seq | tr ' ' '\n' | sort | uniq -c | awk -v t=$tot '{print $2+1 ":" $1/t}' | tr '\n' ' ' && echo
done 
