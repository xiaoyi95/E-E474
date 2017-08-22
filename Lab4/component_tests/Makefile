# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# Makefile for running the component tests
# CSE/EE 474 Lab 4
# 5/20/2017

# Default target: Compiles the motor_test.c program
motor: motor_test.o
	gcc -o ./motor motor_test.c

# Compiles the ds.c program
ds: ds.o
	gcc -o ./ds ds.c

# Runs the program to test the h-bridge driver
test1: motor
	./motor

# Runs the program to test the sensor inputs
test2: ds
	./ds

# removes all object files, executables, and editor files
clean:
	rm -f *.o *~ motor ds
