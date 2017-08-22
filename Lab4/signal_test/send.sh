#!/bin/bash

# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# send.sh
# CSE/EE 474 Lab 4
# 5/20/2017

# This script will compile and run sender.c
# Run this script in a separate terminal after running receive.sh

gcc -o sendsig sender.c
target_PID="$(pgrep -f recsig)"
./sendsig $target_PID
