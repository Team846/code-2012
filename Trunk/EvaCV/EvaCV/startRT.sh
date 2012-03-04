#!/bin/bash
CAMERA=6

v4l2-ctl -d "/dev/video$CAMERA" -c exposure_auto=1 -c focus_auto=1 -c white_balance_temperature_auto=1 -c exposure_absolute=2

cd /home/ubuntu/EvaCV
## args: ./reportCamera <brightThresh> <vertThresh> <camN> <contSend>
./reportTarget 40 80 $CAMERA 1
