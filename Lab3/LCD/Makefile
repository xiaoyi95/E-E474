# Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
# Makefile for lcd.ko
# CSE/EE 474 Lab 3
# 5/5/2017

# Compiler information
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
ccflags-y := -std=gnu99

# Compile lcd.c (for lcd.o object file)
obj-m += lcd.o

# Kernel source directory
KDIR =/home/luut2/bb-kernel/KERNEL
PWD = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
