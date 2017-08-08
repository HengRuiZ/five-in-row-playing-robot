#!/bin/bash
mjpg_streamer -i "input_uvc.so -d /dev/video0 -r 320x240 -f 20 -yuv"  -o "output_http.so -p 8080 -w /www/camwww"
#open http://192.168.168.168:8080/?action=stream
