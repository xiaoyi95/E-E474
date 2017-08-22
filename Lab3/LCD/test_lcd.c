// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// test_lcd.c
// CSE/EE 474 Lab 3
// 5/5/2017

/* test_lcd.c prompts the use for strings to print to the LCD display
 * screen. It sends these strings from the user space to the kernel space.
 * Program takes a max input length of 100 characters per line
 */

// Includes the libraries used
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define NEW_CHAR_DIR "/dev/lcd"  // device driver location
#define MAX_INPUT 100  // Maximum user input

int main(void) {
  int size_buf;  // Size of the string in the input buffer
  int fd;  // Int for linux file location
  char write_buf[MAX_INPUT];  // Buffer for the input from the user

  // Opens the device driver folder, or exits if access denied
  fd = open(NEW_CHAR_DIR, O_RDWR);
  if (fd < 0) {
    printf("File %s cannot be opened\n", NEW_CHAR_DIR);
    exit(1);
  }

  // Prints user instructions to the screen
  printf("This program allows you to write text to the lcd display driver\n");
  printf("Type \\c to clear the display, type \\n to go to a new line\n");
  printf("Enter a string (Max input: %d):\n", MAX_INPUT);

  // Gets input from the user and sends it to the kernel
  while (fgets(write_buf, sizeof(write_buf), stdin) != NULL) {
    size_buf = strlen(write_buf);
    // Removes the newline at the end of the string from the user space
    write_buf[size_buf-1] = '\0';
    size_buf--;
    // Writes the string to the device driver
    write(fd, write_buf, size_buf);
    printf("Successfully wrote to device file\n\n");
    printf("Enter a string (Max input: %d):\n", MAX_INPUT);
  }
	
  close(fd);  // Closes the device driver file
  return 0;
}
