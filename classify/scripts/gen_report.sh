#!/bin/bash

usage() {
  echo "Usage: $0 [-p] [-a] files" >&2
  exit 1
}

while getopts "pa" opt; do
  case $opt in
    p)
      plot=1
      ;;
    a)
      pauc=1
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

files=$@

src="${BASH_SOURCE[0]}"
while [[ -h "$src" ]]; do
  dir="$( cd -P "$( dirname "$src" )" && pwd )"
  src="$(readlink "$src")"
  [[ $src != /* ]] && src="$dir/$src"
done
basedir="$( cd -P "$( dirname "$src" )" && pwd )"

dataf=`mktemp` || exit 1
evalf=`mktemp` || exit 1
plotf=`mktemp` || exit 1

for file in $files; do
  while read line; do
    if [[ $line == '## start ##' ]]; then
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

      $basedir/evalres.m $dataf > $evalf
      auc=`tail -1 $evalf`
      head -n -1 $evalf > $dataf
      mv $dataf $evalf
      echo "reset" >> $plotf
      echo "set multiplot layout 1,2 title '$desc - $tag'" >> $plotf
      echo "set title 'scores'" >> $plotf
      echo "set xlabel 'threshold '" >> $plotf
      echo "set ylabel 'score'" >> $plotf
      echo "set size square" >> $plotf
      echo "plot '$evalf' using 1:2 title 'recall' w l,\\" >> $plotf
      echo "     '$evalf' using 1:4 title 'precision' w l,\\" >> $plotf
      echo "     '$evalf' using 1:5 title 'f-measure' w l,\\" >> $plotf
      echo "     '$evalf' using 1:6 title 'kappa' w l" >> $plotf
      echo "set title 'roc curve (auc = $auc)'" >> $plotf
      echo "set xlabel 'fpr'" >> $plotf
      echo "set ylabel 'tpr'" >> $plotf
      echo "set size square" >> $plotf
      echo "unset key" >> $plotf
      echo "plot '$evalf' using 3:2 w l,\\" >> $plotf
      echo "     x dt 3" >> $plotf
      echo "unset multiplot" >> $plotf

      if [[ -n $plot ]]; then
        gnuplot -p $plotf
      fi
      if [[ -n $pauc ]]; then
        echo "$file $tag $auc"
      fi
    fi
  done < $file
done

rm -f $dataf $evalf $plotf

