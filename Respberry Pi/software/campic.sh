#/bin/bash
fswebcam -d /dev/video0 --no-banner -r 1280x720 ../source/left.jpg
fswebcam -d /dev/video1 --no-banner -r 1280x720 ../source/right.jpg
