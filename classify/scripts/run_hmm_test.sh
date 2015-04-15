#!/bin/bash

usage() {
  echo "Usage: $0 [-i (0:random|1:kmeans)] [-u unseen] -p hmm_test_path data_dir" >&2
  exit 1
}

unseen=1
initm=0
while getopts "p:u:i:" opt; do
  case $opt in
    p)
      prog=$OPTARG
      ;;
    u)
      unseen=$OPTARG
      ;;
    i)
      initm=$OPTARG
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

ddirs=$@

[[ -x $prog ]] || usage
for ddir in $ddirs; do
  [[ -d $ddir ]] || usage
done

tparam=("-n1 -k1"
        "-n1 -k3"
        "-n1 -k6"
        "-n2 -k1"
        "-n2 -k3"
        "-n2 -k6"
        "-n3 -k1"
        "-n3 -k3"
        "-n3 -k6")

for ddir in $ddirs; do
  echo $ddir
  dim=`grep Dimension $ddir/README | awk '{print $2}'`
  name=`echo $ddir | sed 's:^.*/\([^/][^/]*\)/*$:\1:'` 
  fparam="-d $dim -u $unseen -i $initm"

  echo $name

  > errlog
  for ((i = 0; i < ${#tparam[@]}; i++)); do
    tp=${tparam[$i]}
    echo "  $tp"
    file="${name}_hmm_${tp//-/}"
    file="${file// /_}"
    $prog $fparam $tp $ddir 2>> errlog > $file
  done
done

