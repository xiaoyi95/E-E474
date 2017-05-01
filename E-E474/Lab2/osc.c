// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// osc.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* Continually turns a GPIO pin on and off with a 1 us delay 
 * for examination on the oscilloscope.
 */

// Includes the necessary libraries for the functions used
#include <stdio.h>
#include <time.h>
#include "commands.h"

#define GPIO_PIN 60   // GPIO pin number

int main() {
  // Files for the gpio pin
  FILE *sys, *dir, *val;
  // Opens the export file
  sys = fopen("/sys/class/gpio/export", "w");
  if (sys == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }

  fseek(sys, 0, SEEK_SET);
  gpioexport(sys, GPIO_PIN);  // Creates a device folder for the gpio
  dir = createdir(GPIO_PIN);  // Access the direction folder and set to out
  val = createval(GPIO_PIN);  // Access the value folder and set to 0

  // Checking for null file pointers
  if (dir == NULL) {
    printf("did not return file pointer dir\n");
    return 1;
  }
  if (val == NULL) {
    printf("did not return file pointer val\n");
    return 1;
  }

  // Flips the switch on and off
  while(1) {
    gpioout(val, 0);
    usleep(1);
    gpioout(val, 1);
    usleep(1);
  }

}
