// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// filemusic.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* This program creates a pipe, and takes input from the pipe and
 * displays it on the LCD display. For the letters A-Z, the program
 * will play a note on a piano buzzer from A2-E5 respectively. It
 * will also print the appropriate A-G letter on the LCD screen.
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
  // Files for the PWM for the music output
  FILE *note, *per, *duty, *run;
  // Matrix containing the period for each note
  int pitch[26] = {9090909, 8099133, 7645260, 6810597, 6067593, 5727049,
		   5102041, 4545455, 4049567, 3822192, 3405299, 3033704,
		   2863442, 2551020, 2272727, 2024783, 1911132, 1702620,
		   1516875, 1431721, 1275526, 1136364, 1012381, 955566,
		   851310, 758432};
  
  // Opens the slots files for configuring the pwm output
  note = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  // Checks if the slots file was accessed properly
  if (note == NULL) {
    printf("Error: Failed to open slots\n");
    return 1;
  }

  // Sets up the device folders for the proper pwm output pin
  fseek(note, 0, SEEK_SET);
  fprintf(note, "%s", "am33xx_pwm");
  fflush(note);
  fprintf(note, "%s", "bone_pwm_P8_19");
  fflush(note);

  usleep(1000000);

  // Opens the file that controls if the PWM is outputting
  run = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/run", "w");
  // Opens the file that sets the period of the PWM output in nanoseconds
  per = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/period", "w");
  // Opens the file that set the duty cycle of the PWM output in nanoseconds
  duty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/duty", "w");

  // Checks if the run, period, and duty cycle files opened properly
  // Terminates the program if any file failed to open
  if  (run == NULL) {
    printf("Error: Failed to open pwm run file\n");
    return 1;
  } else if (duty == NULL) {
    printf("Error: Failed to open pwm duty file\n");
    return 1;
  } else if (per == NULL) {
    printf("Error: Failed to open pwm period file\n");
    return 1;
  }
  
  // Sets the buffer for each file at the beginning
  fseek(per, 0, SEEK_SET);
  fseek(duty, 0, SEEK_SET);
  fseek(run, 0, SEEK_SET);

  // Initial values for the PWM
  fprintf(per, "%d", 3822250);
  fflush(per);
  fprintf(duty, "%d", 1911125);
  fflush(duty);
  fprintf(run, "%d", 0);
  fflush(run);

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
  char * path = "/root/pipe";
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

  printf("Waiting for input\n");
  
  // Processes each input line from the user until EOF is entered
  while(fgets(word, MAX_INPUT, shared) != NULL) {
    printf("Received input: %s\n", input);
    int length = strlen(input);
    int curr = length;
    // Processes every character in the input string
    while (curr > 0) {
	  // For a newline character, go to a new line on the display
      if (word[length-curr] == 10) {
	    if (count < 40) {	// On the first line, go to the second line
	      sndline(valSel, valRW, valEn, val);
	      count = 39;
	    } else {	// On the second line, refresh to the first line
	      clrdisplay(valSel, valRW, valEn, val);
	      count = -1;
	    }
	    curr--;
      } else {
		// If the input contains A-Z (captial letters), play a note
	    if (word[length-curr] > 64 && word[length-curr] < 91) {
	      fprintf(per, "%d", pitch[word[length-curr]-65]);
	      fflush(per);
	      fprintf(duty, "%d", pitch[word[length-curr]-65]/750);
		  fflush(duty);
		  fprintf(run, "%d", 1);
		  fflush(run);
		  usleep(250000);
		  // Print the musical letter corresponding to the note
		  char note[7] = {'F','G','A','B','C','D','E'};
		  decimalTobinary(note[word[length-curr] % 7], out);
	    } else {
		  // If the input is a dash '-', turn off the music (like a rest)
		  if (word[length-curr] == '-') {
			fprintf(run, "%d", 0);
			fflush(run);
			word[length-curr] = ' ';	// Print a blank space
		  }
		  decimalTobinary(word[length-curr], out);
		}
		writechar(valSel, valRW, valEn, val, out);	// Print to the LCD
	  }
      
	  // If the input exceeds 16 characters on the first line or 56 characters
	  // on the second line, shift the display to the left to continue reading
      if ((count > 15 && count < 40) || (count > 55 && count < 80) ) {
        leftshift(valSel, valRW, valEn, val);
      }
      if (count == 39) {	// Shift at the end of the first line
        rtnhome(valSel, valRW, valEn, val);
        sndline(valSel, valRW, valEn, val);	  
      }	else if (count == 79) {	 // Clear the display at the end of the second line
		usleep(500000);
        clrdisplay(valSel, valRW, valEn, val);
		count = 0;
      }
      curr--;
      count++;
    }
    printf("Waiting for input\n");
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
  fclose(note);
  fclose(per);
  fclose(duty);
  fclose(run);
  return 0;
}

