# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# Makefile
# CSE/EE 474 Lab 2
# 4/21/2017

# default target: program for writing to the display
lcd: lcd.o commands.o
	gcc -o run lcd.o commands.o

# make command for the user input program
send: input.c
	gcc -o send input.c	

# make command for the music playing lcd program
music: filemusic.o commands.o
	gcc -o music filemusic.o commands.o

# make command for the file input program
finput: fileinput.c
	gcc -o finput fileinput.c	
	
# make command for the oscilloscope gpio pin program
osc: osc.o commands.o
	gcc -o osc osc.o commands.o
	
# individual source files:
commands.o: commands.c commands.h
	gcc -c commands.c

osc.o: osc.c commands.c commands.h
	gcc -c osc.c

lcd.o: lcd.c commands.c commands.h
	gcc -c lcd.c
	
filemusic.o: filemusic.c commands.c commands.h
	gcc -c filemusic.c

# removes all object files, executables, and editor files
clean:
	rm -f *.o *~ run send music finput osc pipe

