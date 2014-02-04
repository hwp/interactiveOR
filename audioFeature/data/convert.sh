#!/bin/sh

for file in `cat $1`
do
  gst-launch-1.0 filesrc location=$file ! decodebin ! audioconvert ! audioresample ! audio/x-raw, rate=8000, format=S16LE, channels=1 ! filesink location=${file%MP3}PCM
done

