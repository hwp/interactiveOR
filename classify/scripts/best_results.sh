#!/bin/bash

usage() {
  echo "Usage: $0 -c category -a auc_file ff df vo ao" >&2
  exit 1
}

while getopts "c:a:" opt; do
  case $opt in
    c)
      category=$OPTARG
      ;;
    a)
      aucf=$OPTARG
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

setups=$@

if [[ -z $category ]] || [[ -z $aucf ]] || [[ ! -r $aucf ]] || [[ `wc -w <<< $setups` -ne 4 ]]; then
  usage
fi

src="${BASH_SOURCE[0]}"
while [[ -h "$src" ]]; do
  dir="$( cd -P "$( dirname "$src" )" && pwd )"
  src="$(readlink "$src")"
  [[ $src != /* ]] && src="$dir/$src"
done
basedir="$( cd -P "$( dirname "$src" )" && pwd )"

tempf=`mktemp` || exit 1
for setup in $setups; do
  < $aucf grep $setup | grep $category | sort -nk3 | tail -1 >> $tempf
done

arr=(`awk '{print $1}' $tempf`)
$basedir/plot_roc.sh -c $category -f gp -o $category.gp ${arr[@]}

sed -i "s/${arr[0]}/Feature Fusion/" $category.gp
sed -i "s/${arr[1]}/Decision Fusion/" $category.gp
sed -i "s/${arr[2]}/Visual Only/" $category.gp
sed -i "s/${arr[3]}/Audio Only/" $category.gp

cat $tempf

rm -f $tempf

