# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# Makefile for compiling the programs
# CSE/EE 474 Lab 5
# 6/3/2017

# Default target: runs the basic requirement program
run: basic
	./basic

# Compiles the basic requirement program
basic: basicreq.o
	gcc -o basic basicreq.c

# Compiles the drive program
system: drive.o
	gcc -o drive drive.c

# Compiles the distance sensor program
dsense: distsense.o
	gcc -o dsense distsense.c

# Compiles the bluetooth receiver program
brec: btreceive.o
	gcc -o brec btreceive.c

# Compiles the temperature and light sensor programs
sensor: sensor.o
	gcc -o sensor sensor.c

# Runs the system using the startup script in the root folder
start: system dsense brec sensor
	./startup.sh

# removes all object files, executables, and editor files
clean:
	rm -f *.o *~ drive dsense brec sensor basic
