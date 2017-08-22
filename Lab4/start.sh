#!/bin/bash

# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# start.sh
# CSE/EE 474 Lab 4
# 5/20/2017

# This script will compile and run the autodrive program
# This is meant to be used when the beaglebone is connected to a computer.
# (Primarily used for testing the code)

gcc -o drive drive.c
./drive &
gcc -o sense sense.c
target_PID="$(pgrep -f drive)"
./sense $target_PID
