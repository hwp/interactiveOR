#!/bin/bash

usage() {
  echo "Usage: $0 [-a|-i|categories]" >&2
  exit 1
}

all="mug nonempty bottle metal plastic fragile"
inters="knock push shake"

cats=$@

while getopts "ai" opt; do
  case $opt in
    a)
      cats=$all
      ;;
    i)
      cats=$inters
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

for f in *.fvec
do
  > ${f%.*}.tag
done

for c in $cats
do
  case $c in
    mug)
      for f in 02*.fvec 03*.fvec 06*.fvec 23*.fvec 30*.fvec 
      do 
        if [[ -f $f ]]; then
          echo mug >> ${f%.*}.tag
        fi
      done
      ;;
    nonempty)
      for f in *f_*.fvec 19_*.fvec 26*.fvec 29*.fvec 35*.fvec
      do 
        if [[ -f $f ]]; then
          echo nonempty >> ${f%.*}.tag
        fi
      done
      ;;
    bottle)
      for f in 16*.fvec 19*.fvec 20*.fvec 22*.fvec 21*.fvec 24*.fvec 32*.fvec
      do 
        if [[ -f $f ]]; then
          echo bottle >> ${f%.*}.tag
        fi
      done
      ;;
    metal)
      for f in 09*.fvec 10*.fvec 11*.fvec 12*.fvec 14*.fvec
      do 
        if [[ -f $f ]]; then
          echo metal >> ${f%.*}.tag
        fi
      done
      ;;
    plastic)
      for f in 16*.fvec 17*.fvec 24*.fvec 29*.fvec 31*.fvec 33*.fvec 34*.fvec 35*.fvec
      do 
        if [[ -f $f ]]; then
          echo plastic >> ${f%.*}.tag
        fi
      done
      ;;
    fragile)
      for f in 01*.fvec 02*.fvec 03*.fvec 06*.fvec 07*.fvec 13*.fvec 19*.fvec 20*.fvec 21*.fvec 22*.fvec 23*.fvec 25*.fvec 27*.fvec 28*.fvec 30*.fvec 32*.fvec
      do 
        if [[ -f $f ]]; then
          echo fragile >> ${f%.*}.tag
        fi
      done
      ;;
    shake)
      for f in *shake*.fvec
      do
        if [[ -f $f ]]; then
          echo shake >> ${f%.*}.tag
        fi
      done
      ;;
    knock)
      for f in *knock*.fvec
      do
        if [[ -f $f ]]; then
          echo shake >> ${f%.*}.tag
        fi
      done
      ;;
    push)
      for f in *push*.fvec
      do
        if [[ -f $f ]]; then
          echo shake >> ${f%.*}.tag
        fi
      done
      ;;
  esac
done

