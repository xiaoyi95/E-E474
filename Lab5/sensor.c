// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// sensor.c
// CSE/EE 474 Lab 5
// 6/3/2017

/* sensor.c controls the input from the ADC of the beaglebone from two sensors
 * The temperature sensor takes in a reading in mV, where 220 mV = 22 degrees C
 * The light sensor takes in a reading in mV, where the max is 1800 mV.
 *
 * The program will prompt the user via bluetooth for a temperature type (Celsius, 
 * Fahrenheit, or Kelvin). The user should only input one character as the input 
 * (either c, f, or k). If 5 seconds pass without a valid input, the program will
 * default to Fahrenheit.
 * (For the demo, we decided to let the system default to Fahrenheit as the 
 * bluetooth was struggling to connect within an appropriate time frame, sometimes
 * it took 30+ seconds to start up)
 *
 * The program will send the data via bluetooth to the user, displaying the
 * temperature converted to the requested temperature type, and the brightness
 * percentage (calculated out of 1800 mV). It will send one reading very 3 seconds.
 */

// Includes the libraries used
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <unistd.h>     
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

// Defines the signal numbers for the temperature selection
#define SIG_K 50
#define SIG_C 55
#define SIG_F 60

#define BAUDRATE B9600            // Baudrate for the bluetooth/uart
#define MODEMDEVICE "/dev/ttyO4"  // Pathname for the uart

char in;  // Input character from the user

/*
// Checks which signal was received and updates it accordingly
void sig_handler(int signo) {
  if (signo == SIG_K) {
    in = 'k';
  } else if (signo == SIG_C) {
    in = 'c';
  } else if (signo == SIG_F) {
    in = 'f';
  }
}
*/

int main() {
  FILE *slot, *fd, *fp;  // File holders to access the ADC pins
  char tempStr[5];  // Current value of the temperature sensor
  char lightStr[5];  // Current value of the photoresistor
  char unit;  // Saves temperature scale (C/F/K)
  
  int file, res;  // File number of open, read error check
  struct termios newtio;  // struct for bluetooth settings
  char buf[255];  // Buffer for bluetooth input
  // Sets up the uart file and opens it for reading/writing
  system("echo uart4 > /sys/devices/bone_capemgr.9/slots");
  file = open(MODEMDEVICE, O_RDWR);
  if (file < 0) {
    printf("Error opening file for usrt");
    return -1;
  }
  printf("%d\n", file);

  // Defines the settings flags for the bluetooth
  // BAUDRATE: sets the baud rate; CS8: 8-bit, no parity;
  // CLOCAL: local connection; CREAD: enable receiving characters
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;  // Ignore parity errors
  newtio.c_oflag = 0;       // Gets raw output
  newtio.c_lflag = ICANON;  // Disable echo, enable canonical input
  
  tcflush(file,TCIFLUSH);           // Flushes the uart file
  tcsetattr(file,TCSANOW,&newtio);  // Sets the terminal settings, change occurs immediately

  
  // Opens the file to initialize the ADC input pins
  slot = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  usleep(1000000);
  if (slot == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }
  // Sets up the files for the ADC pins
  fseek(slot, 0, SEEK_SET);
  fprintf(slot, "cape-bone-iio");
  fflush(slot);
  usleep(1000000);
  
  // Asks the user what temperature unit to use, via the uart
  sprintf(buf,"How to display temperature?\n");
  res = write(file, buf, strlen(buf));

  /*
  // Checks for the signals for each sensor, prints an error if
  // we fail to receive the signal
  if (signal(SIG_K, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIG_K\n");
  if (signal(SIG_C, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIG_C\n");
  if (signal(SIG_F, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIG_F\n");
  */
    
  // Waits for five seconds
  usleep(5000000);
  // If five seconds pass without a value user input, default to Fahrenheit
  if (in != 'k' && in != 'c' && in != 'f') {
    in = 'f';
  }
  
  // Tells the user that the tank will start sending them data
  sprintf(buf,"Tank is detecting temperature and brightness levels...\n");
  res = write(file, buf, strlen(buf));
  
  while(1) {
    // Opens the folder containing pin data for the temperature sensor
    fd = fopen("/sys/devices/ocp.3/helper.17/AIN6", "r");
    if (fd == NULL) {
      printf("Error: Failed to open file AIN6\n");
      return 1;
    }
    
    // Opens the folder containing pin data for the light sensor
    fp = fopen("/sys/devices/ocp.3/helper.17/AIN4", "r");
    if (fp == NULL) {
      printf("Error: Failed to open file AIN4\n");
      return 1;
    }
    
    // Reads data from the sensor input pins   
    fscanf(fd, "%s", tempStr);
    fscanf(fp, "%s", lightStr);

    // Converts the temperature input to an int
    int temp = atoi(tempStr);
    double result;
    
    // Calculates the temperature in the correct temperature type
    if (in == 'c') {
      result = (double) temp/10;
      unit = 'C';
    } else if (in == 'k') {
      result = (double) temp/10.0 + 273.15;
      unit = 'K';
    } else {
      result = (double) temp/10.0*1.8 + 32;
      unit = 'F';
    }
   
    // Converts the light input to an int
    int light = atoi(lightStr);
    // Calculates the light percentage
    light = light*100/1800;

    // Sends data to the user terminal
    sprintf(buf, "Temp: %1.2f ~%c and Brightness: %d%%\n", result, unit, light);
    res = write(file, buf, strlen(buf));

    // Close files
    fclose(fd);
    fclose(fp);
    usleep(3000000);  // Wait time before next update
  }

  // Closes the device driver file
  close(file);
  return 0;
}
     
     
