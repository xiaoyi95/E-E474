#!/bin/bash

# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# startup.sh
# CSE/EE 474 Lab 4
# 5/20/2017

# This is the script that is run by a service on boot up. (systemd method)
# It will automatically start the autodrive program.
# The drive and sense programs should already be compiled and in the root folder.

/root/drive &
target_PID="$(pgrep -f drive)"
/root/sense $target_PID
