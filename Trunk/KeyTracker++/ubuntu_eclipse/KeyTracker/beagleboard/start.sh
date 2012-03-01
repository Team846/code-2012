#!/bin/bash
uvcdynctrl -d /dev/video0 -s 'Focus, Auto' 0
uvcdynctrl -d /dev/video0 -s 'White Balance Temperature, Auto' 0
./KeyTracker
