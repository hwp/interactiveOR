#!/bin/sh

for file in `cat $1`
do
  dd if=$file of=${file%.PCM}_1trim.PCM bs=16000 count=2 skip=1
  dd if=$file of=${file%.PCM}_2trim.PCM bs=16000 count=2 skip=3
done

