#!/bin/bash

dataf=`mktemp` || exit 1
evalf=`mktemp` || exit 1
plotf=`mktemp` || exit 1

while true; do
  read -r line || break
  until [[ $line == '## start ##' ]]; do
    read -r line || exit 1
  done

  read -r line || exit 1
  desc=${line#*: }
  desc=${desc% ##}

  read -r line || exit 1
  tag=${line#*: }
  tag=${tag% ##}

  read -r line || exit 1
  until [[ $line == '## end ##' ]]; do
    echo $line >> $dataf
    read -r line || exit 1
  done

  ./evalres.m $dataf > $evalf
  auc=`tail -1 $evalf`
  head -n -1 $evalf > $dataf
  mv $dataf $evalf
  echo "set multiplot layout 1,2 title '$desc - $tag'" >> $plotf
  echo "set title 'scores'" >> $plotf
  echo "set xlabel 'threshold '" >> $plotf
  echo "plot '$evalf' using 1:2 title 'recall' w l,\\" >> $plotf
  echo "     '$evalf' using 1:4 title 'precision' w l,\\" >> $plotf
  echo "     '$evalf' using 1:5 title 'f-measure' w l,\\" >> $plotf
  echo "     '$evalf' using 1:6 title 'kappa' w l" >> $plotf
  echo "set title 'roc curve (auc = $auc)'" >> $plotf
  echo "set xlabel 'fpr'" >> $plotf
  echo "set ylabel 'tpr'" >> $plotf
  echo "unset key" >> $plotf
  echo "plot '$evalf' using 3:2 w l" >> $plotf
  echo "unset multiplot" >> $plotf
  gnuplot -p $plotf
done

rm -f $dataf $evalf $plotf

