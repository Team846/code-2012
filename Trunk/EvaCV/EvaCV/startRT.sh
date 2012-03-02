#!/bin/bash
CAMERA=0

v4l2-ctl -d "/dev/video$CAMERA" -c exposure_auto=1 -c focus_auto=1 -c white_balance_temperature_auto=1 -c exposure_absolute=156

cd /home/ubuntu/EvaCV
./reportTarget 200 $CAMERA