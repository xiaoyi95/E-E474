// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// lcd.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* This program creates a pipe, and takes input from the pipe and
 * displays it on the LCD display. The screen will shift to display
 * each new input it writes to the screen. If the user inputs \n, 
 * the display will move to a new line. If the user inputs \c, the
 * display will clear.
 */

// Includes the necessary libraries for the functions used
#include <stdio.h>
#include <time.h>
#include <string.h>	
#include "commands.h"

#define GPIO_PIN4 49	// GPIO pin for LCD pin 4 (register select)
#define GPIO_PIN5 20	// GPIO pin for LCD pin 5 (read/write)
#define GPIO_PIN6 115	// GPIO pin for LCD pin 6 (enable)
#define GPIO_PIN7 66	// GPIO pin for LCD pin 7 (data 0)
#define GPIO_PIN8 69	// GPIO pin for LCD pin 8 (data 1)
#define GPIO_PIN9 45	// GPIO pin for LCD pin 9 (data 2)
#define GPIO_PIN10 47	// GPIO pin for LCD pin 10 (data 3)
#define GPIO_PIN11 44	// GPIO pin for LCD pin 11 (data 4)
#define GPIO_PIN12 26	// GPIO pin for LCD pin 12 (data 5)
#define GPIO_PIN13 46	// GPIO pin for LCD pin 13 (data 6)
#define GPIO_PIN14 65	// GPIO pin for LCD pin 14 (data 7)
#define MAX_INPUT 100	// Maximum input length from the user

// Inputs: unsigned int val: character ASCII value, 
// int *out: val represented in binary (output to the given pointer)
// Converts an ASCII character to binary
void decimalTobinary(unsigned int val, int *out) {
  unsigned int compare = 1U << 7;
  int i = 7;
  while(i >= 0) {
    out[i] = (val & compare) ? 1 : 0;
    val <<=1;
    i--;
  }
}

int main() {
  // Files to open in the beaglebone to access the GPIO options
  FILE *export, *dirSel, *dirRW, *dirEn, *valSel, *valRW, *valEn;
  FILE *dir[8];
  FILE *val[8];
  
  // Opens export file for the gpio pins, checks if the file opens properly 
  export = fopen("/sys/class/gpio/export", "w");
  if (export == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }
  
  // Creates the device folder for each gpio pin
  fseek(export, 0, SEEK_SET);
  gpioexport(export, GPIO_PIN4);
  gpioexport(export, GPIO_PIN5);
  gpioexport(export, GPIO_PIN6);
  gpioexport(export, GPIO_PIN7);
  gpioexport(export, GPIO_PIN8);
  gpioexport(export, GPIO_PIN9);
  gpioexport(export, GPIO_PIN10);
  gpioexport(export, GPIO_PIN11);
  gpioexport(export, GPIO_PIN12);
  gpioexport(export, GPIO_PIN13);
  gpioexport(export, GPIO_PIN14);

  // Opens the file that controls if the pins are detecting input or output
  dirSel = createdir(GPIO_PIN4);
  dirRW = createdir(GPIO_PIN5);
  dirEn = createdir(GPIO_PIN6);
  dir[0] = createdir(GPIO_PIN7);
  dir[1] = createdir(GPIO_PIN8);
  dir[2] = createdir(GPIO_PIN9);
  dir[3] = createdir(GPIO_PIN10);
  dir[4] = createdir(GPIO_PIN11);
  dir[5] = createdir(GPIO_PIN12);
  dir[6] = createdir(GPIO_PIN13);
  dir[7] = createdir(GPIO_PIN14);

  // Opens the file that controls if the GPIO pins are high or low
  valSel = createval(GPIO_PIN4);
  valRW = createval(GPIO_PIN5);
  valEn = createval(GPIO_PIN6);
  val[0] = createval(GPIO_PIN7);
  val[1] = createval(GPIO_PIN8);
  val[2] = createval(GPIO_PIN9);
  val[3] = createval(GPIO_PIN10);
  val[4] = createval(GPIO_PIN11);
  val[5] = createval(GPIO_PIN12);
  val[6] = createval(GPIO_PIN13);
  val[7] = createval(GPIO_PIN14);  
  
  // Initializes the LCD display so it displays a blinking cursor
  initialize(valSel, valRW, valEn, val);
  
  // Creates a pipe folder to take input from
  char * path= "/root/pipe";
  if (mkfifo(path, 0666) != 0) {
    printf("mkfifo command failed\n");
    return 1;
  }

  printf("...Waiting for sender to connect\n\n");
  
  // Creates a file to read from the pipe
  FILE * shared;
  shared = fopen(path, "r");
  if (shared == NULL) {
    printf("fopen receiver side failed\n");
    return 1;
  }
  
  int out[8];				// Binary representation of each number
  char word[MAX_INPUT];		// Stores input from the user
  char *input = word;		
  int count = 0;			// Current screen space being written to

  printf("Waiting for input...\n");
  
  // Processes each input line from the user until EOF is entered
  while(fgets(word, MAX_INPUT, shared) != NULL) {
    printf("Received input: %s\n", input);
    int length = strlen(input)-1;
    int curr = length;
	// Processes every character in the input string
    while (curr > 0) {
	  // If the input contains \n, go to a new line on the display
      if ((word[length-curr] == 92) && (word[length-curr+1] == 'n')) {
	    if (count < 40) {	// On the first line, go to the second line
	      sndline(valSel, valRW, valEn, val);
	      count = 39;
	    } else {	// On the second line, refresh to the first line
	      clrdisplay(valSel, valRW, valEn, val);
	      count = -1;
	    }
	  curr--;
	  // If the input contains \c, clear the display
      } else if ((word[length-curr] == 92) && (word[length-curr+1] == 'c')) {
	    clrdisplay(valSel, valRW, valEn, val);
	    count = -1;
	    curr--;
	  // Or else print the character normally
      } else {
	    decimalTobinary(word[length-curr], out);
	    writechar(valSel, valRW, valEn, val, out);
      }
      
	  // If the input exceeds 16 characters on the first line or 56 characters
	  // on the second line, shift the display to the left to continue reading
      if ((count > 15 && count < 40) || (count > 55 && count < 80) ) {
        leftshift(valSel, valRW, valEn, val);
      }
      if (count == 39) {	// Shift at the end of the first line
        rtnhome(valSel, valRW, valEn, val);
        sndline(valSel, valRW, valEn, val);	  
      } else if (count == 79) {	 // Clear the display at the end of the second line
	    usleep(500000);
        clrdisplay(valSel, valRW, valEn, val);
	    count = 0;
      }
      curr--;
      count++;
    }
    printf("Waiting for input...\n");
  }
  
  // Closes all files, removes pipe
  unlink(path);
  fclose(shared);
  fclose(export);
  fclose(dirSel);
  fclose(valSel);
  fclose(dirRW);
  fclose(valRW);
  fclose(dirEn);
  fclose(valEn);
  int close = 0;
  while (close < 8) {
	fclose(dir[close]);
	fclose(val[close]);
	close++;
  }
  return 0;
}

