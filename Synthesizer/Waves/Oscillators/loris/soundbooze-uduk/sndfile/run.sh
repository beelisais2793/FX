#!/bin/sh

DUR=5
SLEEP=1

while [ 1 ]
do
  # split capture ... 
  jack_capture -d $DUR -p system:playback* -f wav out1.wav 2> /dev/null
  sleep $SLEEP 
  jack_capture -d $DUR -p system:playback* -f wav out2.wav 2> /dev/null
  sleep $SLEEP 

  ./rev out1.wav rev1.wav
  ./rev out2.wav rev2.wav
  ./inter rev1.wav rev2.wav outr.wav

  paplay outr.wav

  rm -f *.wav
done
