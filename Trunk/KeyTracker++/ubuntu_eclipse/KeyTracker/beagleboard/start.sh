#!/bin/bash
CAMERA=0

# enable white balance auto
# disable exposure auto
# disable focus auto
v4l2-ctl -d "/dev/video$CAMERA" -c exposure_auto=1 -c focus_auto=0 -c white_balance_temperature_auto=1 -c exposure_absolute=156

KEYTRACKER=/home/ubuntu/KeyTracker/beagleboard/KeyTracker
#KEYTRACKER=./KeyTracker_gui

$KEYTRACKER $CAMERA
