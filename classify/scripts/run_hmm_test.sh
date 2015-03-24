#!/bin/bash

usage() {
  echo "Usage: $0 -p hmm_test_path data_dir" >&2
  exit 1
}

while getopts "p:" opt; do
  case $opt in
    p)
      prog=$OPTARG
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

ddir=$1

[[ -d $ddir ]] && [[ -x $prog ]] || usage

dim=`grep Dimension $ddir/README | awk '{print $2}'`
name=`echo $ddir | sed 's:^.*/\([^/][^/]*\)/*$:\1:'` 
echo $name

> errlog
$prog -d $dim -n 1 -k 1 -c 1 $ddir 2>> errlog > ${name}_hmm_n1k1
$prog -d $dim -n 1 -k 3 -c 1 $ddir 2>> errlog > ${name}_hmm_n1k3
$prog -d $dim -n 3 -k 1 -c 1 $ddir 2>> errlog > ${name}_hmm_n3k1
$prog -d $dim -n 3 -k 3 -c 1 $ddir 2>> errlog > ${name}_hmm_n3k3
$prog -d $dim -n 3 -k 6 -c 1 $ddir 2>> errlog > ${name}_hmm_n3k6

