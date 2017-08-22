#!/bin/bash

# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# startup.sh
# CSE/EE 474 Lab 5
# 6/3/2017

# This is the script that is run by a service on boot up. (systemd method)
# It will automatically start the geosensing car programs, in the right order.
# The programs should already be compiled and in the root folder.

sleep 5

#insmod slcd.ko
#mknod /dev/slcd c 240 0

/root/drive &
drive_PID="$(pgrep -f drive)"
/root/dsense $drive_PID
/root/sensor
#sensor_PID="$(pgrep -f sensor)"
/root/brec $drive_PID #sensor_PID
