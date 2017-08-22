// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// lcd.h
// CSE/EE 474 Lab 3
// 5/5/2017

// Header file that includes the information needed for lcd.c

#ifndef LCD_H
#define LCD_H

// Includes the needed linux kernel libraries
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define DEVICE_NAME "lcd"  // Name of the device
#define MAX_BUFF 100  // Maximum buffer space for the input

#define SER 49      // GPIO pin number for the Serial In to the shift register
#define SRCLK 20    // GPIO pin number for the SRCLK shift register input
#define RCLK 115    // GPIO pin number for the RCLK shift register input
#define ENABLE 112  // GPIO pin number for the enable for the LCD display
#define REGSEL 60   // GPIO pin number for the register select for the LCD

// Struct for the character device contents
// Has a buffer to store input strings and a semaphore
struct fake_device {
  char data[MAX_BUFF];
  struct semaphore sem;
} virtual_device;

static struct cdev* mcdev;  // Stores the character device struct
static dev_t dev_num;       // Stores the device's major number

// Initialization instructions for the initialization sequence
static int init1[8] = {0, 0, 1, 1, 0, 0, 0, 0};
static int init2[8] = {0, 0, 1, 1, 1, 1, 0, 0};
static int init3[8] = {0, 0, 0, 0, 0, 1, 1, 1};

// LCD commands for the instruction mode
static int clrd[8] = {0, 0, 0, 0, 0, 0, 0, 1};  // clear display
static int don[8] = {0, 0, 0, 0, 1, 1, 1, 1};   // display on (blinking cursor)
static int doff[8] = {0, 0, 0, 0, 1, 0, 0, 0};  // display off
static int fset[8] = {0, 0, 1, 1, 1, 0, 0, 0};  // function set (2 line display)
static int entrym[8] = {0, 0, 0, 0, 0, 1, 1, 0};  // entry mode set (increment)
static int shiftleft[8] = {0, 0, 0, 1, 1, 0, 0, 0};  // Shifts the screen left
static int returnhome[8] = {0, 0, 0, 0, 0, 0, 1, 0};  // Return home
static int secondline[8] = {1, 1, 0, 0, 0, 0, 0, 0};  // Cursor to second line

static int hold[8] = {0, 1, 0, 0, 0, 0, 0, 1};  // Character to be printed
static int degree[8] = {1, 1, 0, 1, 1, 1, 1, 1};  // Prints the degree sign
static int cursor = 0;  // Current cursor location

// Function declartions for lcd.c
static int __init driver_entry(void);
static void __exit driver_exit(void);
static int  device_open(struct inode*, struct file*);
static int device_close(struct inode*, struct file *);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);
static void initial(void);
static void decToBin(unsigned int val, int *out);
static void enable(unsigned pin);
static void send(int instr[8]);
static void writechar(char curr);
static void lcdwrite(int size);

// Array of pins to be requested and freed
// All pins defaulted to output and value low
static struct gpio pins[] = {
  {SER, GPIOF_OUT_INIT_LOW, "data"},
  {SRCLK, GPIOF_OUT_INIT_LOW, "clock"},
  {RCLK, GPIOF_OUT_INIT_LOW, "latch"},
  {ENABLE, GPIOF_OUT_INIT_LOW, "enable"},
  {REGSEL, GPIOF_OUT_INIT_LOW, "regsel"}
};

// Defines the methods for file operations
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_close
};

#endif
