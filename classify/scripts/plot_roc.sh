#!/bin/bash

usage() {
  echo "Usage: $0 -c category [-f format] [-t title] -o output files" >&2
  exit 1
}

format=eps

while getopts "c:f:t:o:" opt; do
  case $opt in
    c)
      category=$OPTARG
      ;;
    f)
      format=$OPTARG
      ;;
    t)
      title=$OPTARG
      ;;
    o)
      output=$OPTARG
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

files=$@

if [[ -z $category ]] || [[ -z $output ]] || [[ -z $files ]]; then
  usage
fi

if [[ -z $title ]]; then
  title=$category
fi

src="${BASH_SOURCE[0]}"
while [[ -h "$src" ]]; do
  dir="$( cd -P "$( dirname "$src" )" && pwd )"
  src="$(readlink "$src")"
  [[ $src != /* ]] && src="$dir/$src"
done
basedir="$( cd -P "$( dirname "$src" )" && pwd )"

declare -A datas
declare -A aucs

tempf=`mktemp` || exit 1
for file in $files; do
  dataf=`mktemp` || exit 1
  datas[$file]=$dataf
  while read line; do
    if [[ $line == '## start ##' ]]; then
      read -r line || exit 1
      desc=${line#*: }
      desc=${desc% ##}

      read -r line || exit 1
      tag=${line#*: }
      tag=${tag% ##}

      read -r line || exit 1
      > $tempf
      until [[ $line == '## end ##' ]]; do
        echo $line >> $tempf
        read -r line || exit 1
      done

      if [[ $tag == $category ]]; then
        $basedir/evalres.m $tempf > $dataf
        aucs[$file]=`tail -1 $dataf`
        head -n -1 $dataf > $tempf
        mv $tempf $dataf
      fi
    fi
  done < $file
done

if [[ $format == 'gp' ]]; then
  plotf=$output
else
  plotf=`mktemp` || exit 1
fi

> $plotf
echo "#genearted by plot_roc.sh" >> $plotf

case $format in
  eps) 
    echo "set terminal postscript $format enhanced color solid" >> $plotf
    echo "set output '$output'" >> $plotf
    ;;
  png)
    echo "set terminal pngcairo enhanced" >> $plotf
    echo "set output '$output'" >> $plotf
    ;;
  gp)
    echo "set term tikz size 10,10" >> $plotf
    ;;
  *)
    fp=-p
    ;;
esac

echo "#set title '$title'" >> $plotf
echo "set key right bottom spacing 2.5" >> $plotf
echo "set xlabel 'false positive rate' offset 0,-.5" >> $plotf
echo "set ylabel 'true positive rate' offset -2,0" >> $plotf
echo "set size square" >> $plotf
echo -n "plot x dt 3 notitle" >> $plotf
for file in $files; do
  echo -n ", '-' w l lw 3 title '${file} (${aucs[$file]})'" >> $plotf
done
echo >> $plotf

for file in $files; do
  awk '{print $3,$2}' ${datas[$file]} >> $plotf
  echo "e" >> $plotf
done

if [[ $format != 'gp' ]]; then
  gnuplot $fp $plotf
  rm -f $plotf
fi

for dataf in "${datas[@]}"; do
  rm -f $dataf
done
rm -f $tempf

