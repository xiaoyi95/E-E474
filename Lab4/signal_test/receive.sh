#!/bin/bash

# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# receive.sh
# CSE/EE 474 Lab 4
# 5/20/2017

# This script will compile and run receiver.c
# Run this script in a separate terminal before running send.sh

gcc -o recsig receiver.c
./recsig
