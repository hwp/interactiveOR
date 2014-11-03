#!/bin/sh

for file in `cat $1`
do
  f=${file#*data/}
  n=${f%%/*}
  g=${f#*audio/}
  a=${g%%/*}
  printf "$n $a "
  ../build/src/audiomap -w256 -s128 -fS16_LE -o ../build/objectsfull.som $file
done

