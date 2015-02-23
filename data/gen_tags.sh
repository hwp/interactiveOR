#!/bin/bash

for f in *.fvec
do
  > ${f%.*}.tag
done

for f in 02*.fvec 03*.fvec 06*.fvec 08*.fvec 23*.fvec 30*.fvec 
do 
  echo mug >> ${f%.*}.tag
done

for f in *f_*.fvec 19_*.fvec 26*.fvec 29*.fvec 35*.fvec
do 
  echo nonempty >> ${f%.*}.tag
done

for f in 15*.fvec 16*.fvec 19*.fvec 20*.fvec 22*.fvec 21*.fvec 24*.fvec 32*.fvec
do 
  echo bottle >> ${f%.*}.tag
done

for f in 09*.fvec 10*.fvec 11*.fvec 12*.fvec 14*.fvec
do 
  echo metal >> ${f%.*}.tag
done

for f in 16*.fvec 17*.fvec 24*.fvec 29*.fvec 31*.fvec 33*.fvec 34*.fvec 35*.fvec
do 
  echo plastic >> ${f%.*}.tag
done

for f in 01*.fvec 02*.fvec 03*.fvec 06*.fvec 07*.fvec 13*.fvec 19*.fvec 20*.fvec 21*.fvec 22*.fvec 23*.fvec 25*.fvec 27*.fvec 28*.fvec 30*.fvec 32*.fvec
do 
  echo fragile >> ${f%.*}.tag
done

