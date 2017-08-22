# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# Makefile for running the lcd display driver
# CSE/EE 474 Lab 3
# 5/5/2017

# Default target: Compiles the test_lcd.c program
lcd: test_lcd.o
	gcc -o ./lcd test_lcd.c

# Compiles the sensor.c program
sensor: sensor.o
	gcc -o ./sensor sensor.c

# Runs the program to write to the LCD display
run1: lcd
	./lcd

# Runs the program with the temperature sensor information
run2: sensor
	./sensor

# Inserts the kernel module for the corresponding driver
ko1:
	insmod lcd.ko

ko2:
	insmod slcd.ko

# Removes the kernel module for the corresponding driver
rko1:
	rmmod lcd.ko

rko2:
	rmmod slcd.ko

# Creates the driver in the dev folder
dev1:
	mknod /dev/lcd c 240 0

dev2:
	mknod /dev/slcd c 240 0

# Removes the driver in the dev folder
rdev1:
	rm /dev/lcd

rdev2:
	rm /dev/slcd

# Removes executables and object files
clean:
	rm -f *.o *~ lcd sensor
