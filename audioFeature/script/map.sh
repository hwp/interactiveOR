#!/bin/sh

for file in `cat $1`
do
  f=${file#*data/}
  n=${f%%/*}
  g=${f#*audio/}
  a=${g%%/*}
  printf "$n $a "
  ../build/src/audiomap -w1024 -s256 -fS16_LE -o ../build/audio.som $file
done

