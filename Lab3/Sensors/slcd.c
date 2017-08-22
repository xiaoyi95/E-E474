// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// slcd.c
// CSE/EE 474 Lab 3
// 5/5/2017

/* slcd.c contains all of the functions of lcd.c, with some additions to run
 * the temperature sensor display. Typing "\f" will move the cursor to the
 * 7th spot on the first line, where the temperature needs to be printed.
 * Typing "\s" will print the cursor to the 13th spot on the second line, where
 * the brightness level will be printed.
 * The cursor is turned off on this version, and the delay between characters
 * printing is removed.
 */

#include "slcd.h"  // Include the header file for the slcd kernel

// Runs when the module is inserted into the beaglebone
// Allocates space for the character device and prints the major number to the kernel messages
// Initializes the LCD display
static int __init driver_entry(void) {
  // Registers the device with the system and dynamically allocates memory for the device
  // Prints error if the allocation fails
  int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
  if (ret < 0) {
    printk(KERN_ALERT "slcd: Failed to allocate a major number\n");
    return ret;
  }

  // Prints the major number of the device to kernel messages
  printk(KERN_INFO "slcd: major number is %d\n", MAJOR(dev_num));
  printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

  // Creates and initializes the cdev structure for the character device
  mcdev = cdev_alloc();
  mcdev->ops = &fops;
  mcdev->owner = THIS_MODULE;

  // After creating cdev, adds it to kernel
  ret = cdev_add(mcdev, dev_num, 1);
  if (ret < 0) {
    printk(KERN_ALERT "slcd: unable to add cdev to kernerl\n");
    return ret;
  }
  
  // Initialize the semaphore
  sema_init(&virtual_device.sem, 1);

  // Requests and exports the GPIO pins
  // Default values on output and data low
  if (gpio_request_array(pins, 5)) {
    printk(KERN_ALERT "slcd: request array did not work");
    return 1;
  }
  
  // Calls the initialization sequence
  initial();
  
  return 0;
}

// Runs when the module is removed from the beaglebone
// Frees the usage of the pins and the character device driver
static void __exit driver_exit(void) {
  cdev_del(mcdev);
  unregister_chrdev_region(dev_num, 1);
  printk(KERN_ALERT "slcd: successfully unloaded\n");
  
  gpio_free_array(pins, 5);
}

// Runs when the device file is opened
// inode reference to file on disk, struct file represents an abstract
// checks to see if file is already open (semaphore is in use)
// prints error message if device is busy.
int device_open(struct inode *inode, struct file* filp) {
  // Checks if the file is open, locks if you get access to the file
  if (down_interruptible(&virtual_device.sem) != 0) {
    printk(KERN_ALERT "slcd: could not lock device during open\n");
    return -1;
  }
  printk(KERN_INFO "slcd: device opened\n");
  return 0;
}

// Runs when the device file is closed
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
  up(&virtual_device.sem);
  printk(KERN_INFO "slcd: closing device\n");
  return 0;
}

// Runs when the user tries to read information from the device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
  printk(KERN_INFO "slcd: Reading from device...\n");
  // Copies userspace data from the kernel buffer to the userspace buffer
  return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}


// Runs when the user wants to write information to the device
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
  printk(KERN_INFO "slcd: writing to device...\n");
  // Copies data from the userspace buffer to the kernel space buffer
  if (copy_from_user(virtual_device.data, bufSource, bufCount)) {
    printk(KERN_DEBUG "failed to copy from user space\n");
    return -EFAULT;
  }
  // Checks if the maximum buffer count was exceeded
  if (bufCount > MAX_BUFF) {
    printk(KERN_DEBUG "inputted a length larger than the max input");
    return -EINVAL;
  }

  // Writes the contents of the buffer to the LCD screen
  lcdwrite(bufCount);
	
  return bufCount;
}

// Runs a set of commands to intiialize the screen
static void initial(void) {
  msleep(15);
  send(init1);  // Function set
  msleep(5);
  send(init1);  // Function set
  msleep(1);
  send(init1);  // Function set
  send(init2);  // Function set
  send(doff);   // Display off
  send(clrd);   // Clear display
  send(init3);  // Entry mode set
  send(don);    // Display on
  
  send(fset);   // Function set: set to 2 line display
  send(don);    // Display on: no cursor
  send(entrym); // Entry mode set: increment, shift off
  send(clrd);   // Clear display
}

// Inputs: unsigned int val = character to convert, int *out = binary matrix
// Converts a character's ASCII number value to binary
static void decToBin(unsigned int val, int *out) {
  unsigned int compare = 1U << 7;
  int i = 0;
  while(i < 8) {
    out[i] = (val & compare) ? 1 : 0;
    val <<=1;
    i++;
  }
}

// Flips the inputted unsigned pin number to high one millisecone
// Flips the pin low afterwards (looks like a pulse)
static void enable(unsigned pin) {
  gpio_set_value(pin, 1);
  msleep(1);
  gpio_set_value(pin, 0);
}

// Sends the current command saved in int instr[8] to the board
// (instr[0] = DB7, instr[1] = DB6, ..., instr[7] = DB0)
static void send(int instr[8]) {
  int count = 0;
  while (count < 8) {
    gpio_set_value(SER, instr[count]);  // Goes to serial line of data
    msleep(1);
    enable(SRCLK);  // Shifts the shift register
    count++;
  }
  enable(RCLK);  // Latches the content of the shift register to the output
  enable(ENABLE);  // Enable for the LCD, reads data from the shift register
}

// Writes a character to the LCD display
// Updates the int that represents the position of the cursor
static void writechar(char curr) {
  // Because the binary for the "~" prints as an arrow, we used this to
  // represent our degree sign instead
  if (curr == '~') {
    send(degree);
  } else {
    decToBin(curr, hold);  //Convert and print normally
    send(hold);
  }
  cursor++;
  // If the screen reset to the beginning
  if (cursor > 79) {
    cursor = 0;
  }
}

// Input: in representing which part of the buffer we are reading
// Handles the backslash commands from the user
// Normal usage:  \c = clear display, \n = new line
// For sensors:   \f = first line, position 7, \s = second line, position 13
static void slashcommand(int num) {
  // If the user inputs new line and the cursor is on the first line of the display
  if (virtual_device.data[num] == 'n' && cursor < 40) {
    gpio_set_value(REGSEL, 0);
    send(returnhome);
    send(secondline);
    cursor = 40;
    gpio_set_value(REGSEL, 1);
    // If the user inputs a new line and the cursor is on the second line of the
    // display, or if the user wants to clear the display
  } else if ((virtual_device.data[num] == 'c') || (virtual_device.data[num] == 'n' && cursor >= 40)) {
    gpio_set_value(REGSEL, 0);
    send(clrd);
    cursor = 0;
    gpio_set_value(REGSEL, 1);
    // If the user wants to write the temperature on the first line,
    // moves cursor to the position where the temperature reading should be read
  } else if (virtual_device.data[num] == 'f') {
    gpio_set_value(REGSEL, 0);
    send(l1c7);
    cursor = 6;
    gpio_set_value(REGSEL, 1);
    // If the user wants to write the brightness percentage on the second line,
    // moves cursor to the position where the brightness reading should be read
  } else if (virtual_device.data[num] == 's') {
    gpio_set_value(REGSEL, 0);
    send(l2c13);
    cursor = 52;
    gpio_set_value(REGSEL, 1);
  }
}

// Takes an int representing the size of the string in the buffer and writes
// the string to the lcd screen. The screen will shift if the input string goes
// past the 16 characters displayed on the screen
static void lcdwrite(int size) {
  int count = 0;  // Keeps track of which character we are printing
  gpio_set_value(REGSEL, 1);  // LCD write mode

  // Loops through the string in the buffer
  while (count < size) {
    // Executes the backslash command, does not print the backslash
    // command to the screen
    if (virtual_device.data[count] == '\\') {
      count++;
      slashcommand(count);
    } else {
      // Clears the screen if the cursor goes back to 0
      if (cursor == 0) {
	gpio_set_value(REGSEL, 0);
	send(clrd);
	gpio_set_value(REGSEL, 1);
      }
      // Writes the character to the screen
      writechar(virtual_device.data[count]);
      // Shifts the screen to follow the cursor when it writes past 16
      if ((cursor > 15 && cursor < 40) || (cursor > 55 && cursor < 80)) {
	gpio_set_value(REGSEL, 0);
	send(shiftleft);
	gpio_set_value(REGSEL, 1);
      }
      // Shifts screen back to the beginning when it writes past 40 on the top line
      if (cursor == 40) {
	gpio_set_value(REGSEL, 0);
	send(returnhome);
	send(secondline);
	gpio_set_value(REGSEL, 1);
      }
    }
    count++;
  }
  
  gpio_set_value(REGSEL, 0);  // LCD instruction mode
}


MODULE_LICENSE("GPL");
module_init(driver_entry);  // Calls driver_entry() on insmod
module_exit(driver_exit);  // Calls driver_exit() on rmmod
