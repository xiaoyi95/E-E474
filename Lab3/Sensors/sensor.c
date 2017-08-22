// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// sensor.c
// CSE/EE 474 Lab 3
// 5/5/2017

/* sensor.c controls the input from the ADC of the beaglebone from two sensors
 * The temperature sensor takes in a reading in mV, where 220 mV = 22 degrees C
 * The light sensor takes in a reading in mV, where the max is 1800 mV.
 * The program will prompt the user for a temperature type (Celsius, Fahrenheit,
 * or Kelvin). The user should only input one character as the input (either
 * c, C, f, F, k, or K).
 * The LCD screen will display the temperature converted to the requested
 * temperature type, and the brightness percentage (calculated out of 1800 mV).
 */

// Includes the libraries used
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <unistd.h>     
#include <fcntl.h>     

#define NEW_CHAR_DIR "/dev/slcd"  // device driver location
#define MAX_INPUT 100  // Maximum input to the kernel space
#define REPEAT 30  // Number of times the screen will read data from the sensors

int main() {
  int size_buf;  // Size of the string in the input buffer
  int fdev;  // Int for the linux file location of the device driver
  char write_buf[MAX_INPUT];  // Buffer for the input to the kernel space

  // Opens device driver folder, or exits if access denied
  fdev = open(NEW_CHAR_DIR, O_RDWR);
  if (fdev < 0) {
    printf("File %s cannot be opened\n", NEW_CHAR_DIR);
    exit(1);
  }

  FILE *slot, *fd, *fp;  // File holders to access the ADC pins
  char tempStr[5];  // Current value of the ADC AIN1 pin
  char lightStr[5];  // Current value of the ADC AIN0 pin
  char unit;  // Saves temperature scale (C/F/K)

  // Opens the file to initialize the ADC input pins
  slot = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  usleep(1000000);
  if (slot == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }

  // Writes to the slots file to create a pin file for the ADC inputs
  fseek(slot, 0, SEEK_SET);
  fprintf(slot, "cape-bone-iio");
  fflush(slot);
  usleep(1000000);

  // Asks the user for the temperature type they want to display
  printf("How to display temperature?\n");
  printf("K for Kelvin, F for Fahrenheit and C for Celsius: ");

  // Stores the input from the user for the temperature type
  char in[3];
  fgets(in, 3, stdin);
  in[0] = tolower(in[0]);

  // If the user gives an invalid input, requests again
  while(in[0] != 'k' && in[0] != 'c' && in[0] != 'f') {
    printf("Invalid input.\n");
    printf("K for Kelvin, F for Fahrenheit and C for Celsius: ");
    fgets(in, 3, stdin);
    in[0] = tolower(in[0]);
  }

  // Writes the sensor labels to the screen
  write(fdev, "Temp: \\n", 8);
  write(fdev, "Brightness: ", 12);
  
  int times = 0;  // Counts the number of times data has been sampled
  while(times < REPEAT) {
    // Opens the folder containing pin data for the temperature sensor
    fd = fopen("/sys/devices/ocp.3/helper.15/AIN1", "r");
    if (fd == NULL) {
      printf("Error: Failed to open file AIN1\n");
      return 1;
    }
    
    // Opens the folder containing pin data for the light sensor
    fp = fopen("/sys/devices/ocp.3/helper.15/AIN0", "r");
    if (fp == NULL) {
      printf("Error: Failed to open file AIN0\n");
      return 1;
    }
    // Reads data from the sensor input pins   
    fscanf(fd, "%s", tempStr);
    fscanf(fp, "%s", lightStr);

    // Converts the temperature input to an int
    int temp = atoi(tempStr);
    double result;
    
    // Calculates the temperature in the correct temperature type
    if (in[0] == 'c') {
      result = (double) temp/10;
      unit = 'C';
    } else if (in[0] == 'k') {
      result = (double) temp/10.0 + 273.15;
      unit = 'K';
    } else {
      result = (double) temp/10.0*1.8 + 32;
      unit = 'F';
    }

    // Formats the string to send to the lcd kernel
    char send[20];
    sprintf(send, "\\f%1.2f ~%c", result, unit);

    // Converts the light input to an int
    int light = atoi(lightStr);
    // Calculates the light percentage
    light = light*100/1800;

    // Formates the string to send to the lcd kernel
    char bright[20];
    sprintf(bright, "\\s%d%%", light);

    // Calculates the length of the strings we are send
    int tsize = strlen(send);
    int lsize = strlen(bright);

    // Sends the data to the lcd kernel (writes info to the lcd display)
    write(fdev, send, tsize);
    write(fdev, bright, lsize);
    times++;
    usleep(250000);  // Wait time before next update

    // Close files
    fclose(fd);
    fclose(fp);
  }

  // Clears the display after we are finished running
  char * clear = "\\c";
  int clen = strlen(clear);
  write(fdev, clear, clen);

  // Closes the device driver file
  close(fdev);
  return 0;
}
     
     
