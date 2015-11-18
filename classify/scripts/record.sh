#!/bin/bash 

audio_device="hw:0,0"
video_device="/dev/video0"

actions="knock1 knock2 push shake"
num_trials=5


name=""
read -p "Name of Object (press return to exit): " name
while [[ -n "$name" ]]; do
  for action in $actions; do
    echo "Object $name; Action $action"
    gst-launch-1.0 -e v4l2src device=${video_device} ! videoscale ! autovideosink
    for ((i=1; i<=$num_trials; i++ )); do
      echo "Record $action # $i"
      sleep 1
      echo -e "\a"
      timeout -sINT 4 gst-launch-1.0 -e v4l2src device=${video_device} \
        ! videoconvert ! videorate ! video/x-raw,width=640,height=480,framerate=10/1 \
        ! tee name=t ! queue ! mux. \
        alsasrc device=${audio_device} ! audioconvert ! audioresample \
        ! audio/x-raw,rate=16000,channels=1 ! lamemp3enc ! queue ! mux. \
        avimux name=mux ! filesink location="${name}_${action}_${i}.avi" \
        t. ! timeoverlay ! videoscale ! autovideosink
    done
  done
  name=""
  read -p "Name of Object: " name
done

