#!/bin/bash

for f in *.fvec
do
  > ${f%.*}.tag
done

for f in 02*.fvec 03*.fvec 06*.fvec 23*.fvec 30*.fvec 
do 
  if [[ -f $f ]]; then
    echo mug >> ${f%.*}.tag
  fi
done

for f in *f_*.fvec 19_*.fvec 26*.fvec 29*.fvec 35*.fvec
do 
  if [[ -f $f ]]; then
    echo nonempty >> ${f%.*}.tag
  fi
done

for f in 16*.fvec 19*.fvec 20*.fvec 22*.fvec 21*.fvec 24*.fvec 32*.fvec
do 
  if [[ -f $f ]]; then
    echo bottle >> ${f%.*}.tag
  fi
done

for f in 09*.fvec 10*.fvec 11*.fvec 12*.fvec 14*.fvec
do 
  if [[ -f $f ]]; then
    echo metal >> ${f%.*}.tag
  fi
done

for f in 16*.fvec 17*.fvec 24*.fvec 29*.fvec 31*.fvec 33*.fvec 34*.fvec 35*.fvec
do 
  if [[ -f $f ]]; then
    echo plastic >> ${f%.*}.tag
  fi
done

for f in 01*.fvec 02*.fvec 03*.fvec 06*.fvec 07*.fvec 13*.fvec 19*.fvec 20*.fvec 21*.fvec 22*.fvec 23*.fvec 25*.fvec 27*.fvec 28*.fvec 30*.fvec 32*.fvec
do 
  if [[ -f $f ]]; then
    echo fragile >> ${f%.*}.tag
  fi
done

