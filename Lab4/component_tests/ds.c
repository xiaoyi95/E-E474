// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// ds.c
// CSE/EE 474 Lab 4
// 5/20/2017

/* ds.c (short for distance_sensor) tests the functionality of the 
 * distance sensors. The programs reads from the pins the number of times
 * specified by the user and prints those values to the screen.
 * The reading is in milliVolts, the max value for a reading is 1600.
 */

// Includes the libraries used
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <unistd.h>     
#include <fcntl.h> 

// ADC pin number for each of the sensor inputs
#define FRONT 0
#define BACK 1
#define RIGHT 2
#define LEFT 3

#define REPEAT 1000  // Number of reads

// Function declaration
void ADC_read(int pin, int *val);

int main() {
  FILE *slots;  // Slots file for enabling the ADC pins
  int df, db, dr, dl;  // Values of the sensor inputs

  // Opens the slots file for using the ADC pins
  slots = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }

  // Enables the ADC pins for taking input
  fseek(slots, 0, SEEK_SET);
  fprintf(slots, "cape-bone-iio");
  fflush(slots);

  int count = 0;

  // Reads and prints the values of the ADC pins a number of times
  // specified by the user
  while (count < REPEAT) {
    ADC_read(FRONT, &df);
    ADC_read(BACK, &db);
    ADC_read(RIGHT, &dr);
    ADC_read(LEFT, &dl);
    
    printf("Front sensor value is: %d\n", df);
    printf("Back sensor value is: %d\n", db);
    printf("Right sensor value is: %d\n", dr);
    printf("Left sensor value is: %d\n\n", dl);

    usleep(50000);
    count++;
  }

  fclose(slots);  // Close the slots file
  return 0;
}

// Takes a ADC pin number int pin as an input and reads the value of that
// pin. Saves that value (a voltage in mV) to the integer pointer val
void ADC_read(int pin, int *val) {
  char ainBuf[40];  // pathname of the ADC pin
  FILE *fd;  // File pointer to the pathname of the ADC pin
  char dStr[5];  // Character buffer for reading the input

  // Creates and opens a file to read from the ADC pin
  sprintf(ainBuf, "/sys/devices/ocp.3/helper.15/AIN%d", pin);
  fd = fopen(ainBuf, "r");
  if (fd == NULL) {
    printf("Error: Failed to open file AIN%d\n", pin);
    exit(1);
  }

  // Reads the value in millvolts from the pin
  fscanf(fd, "%s", dStr);
  // Converts the value to an int and save it to the input pointer
  *val = atoi(dStr);
  // Closes the ADC file
  fclose(fd);
}
