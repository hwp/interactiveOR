#!/bin/bash

usage() {
  echo "Usage: $0 file" >&2
  exit 1
}

file=$1

if [[ ! -e $file ]]; then
  usage
fi

categories="metal fragile bottle nonempty plastic mug"
parameters="n1k1 n1k3 n3k3 n3k6"
behaviors="all knock shake"
modalities="video audio bimod"

width=8

for c in $categories; do
  echo $c
  printf "%${width}s %${width}s" "" ""
  for b in $behaviors; do
    printf " %${width}s" $b
  done
  echo

  for m in $modalities; do
    for p in $parameters; do
      printf "%${width}s %${width}s" $m $p
      for b in $behaviors; do
        a=`grep "${b}_${m}_hmm_${p} ${c}" $file | awk '{print $3}'`
        printf " %${width}s" $a
      done
      echo
    done
  done
  echo
done

