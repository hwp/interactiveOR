#!/bin/bash

usage() {
  echo "Usage: $0 -w window_size -s shift_size [-p pattern] filelist outdir" >&2
  exit 1
}

cmd="$0 $@"

while getopts "w:s:p:" opt; do
  case $opt in
    w)
      wsize=$OPTARG
      ;;
    s)
      ssize=$OPTARG
      ;;
    p)
      pattern=$OPTARG
      ;;
    *)
      usage
      ;;
  esac
done
shift $((OPTIND-1))

filelist=$1
outdir=$2

nure='^[0-9]+$'
[[ $wsize =~ $nure ]] && [[ $ssize =~ $nure ]] || usage

[[ -n $outdir ]] || usage

if ! [[ -f $filelist ]]; then
  echo "Error: file $filelist does not exist" >&2
  exit 1
fi

if [[ -d $outdir ]]; then
  echo "Warning: directory $outdir exists and will be overwritted." >&2
elif [[ -e $outdir ]]; then
  echo "Error: file $outdir exists" >&2
  exit 1
else
  mkdir "$outdir"
fi

echo "# this file and the data in this folder is generated by the following command" > "${outdir}/README"
echo "# $cmd" >> "${outdir}/README"
echo "Feature: STFT" >> "${outdir}/README"
echo "Dimension: $((wsize / 2 + 1))" >> "${outdir}/README"
echo "Format: double" >> "${outdir}/README"
echo "Window Size: $wsize" >> "${outdir}/README"
echo "Shift Size: $ssize" >> "${outdir}/README"

echo "# this file records the raw audio file of the feature data" > "${outdir}/SOURCES"

id=0
for file in `grep "$pattern" "$filelist"`; do
  class=`echo $file | awk -F'/' '{print $7}'`
  dest="${class}_${id}.fvec"
  echo "$dest < $file" >> "${outdir}/SOURCES"
  gst-launch-1.0 -q filesrc location="$file" ! audio/x-raw,rate=8000,channels=1,format=S16LE ! audioconvert ! stft wsize=$wsize ssize=$ssize location="$outdir/$dest" silent=true ! fakesink
  id=$((id + 1))
done

