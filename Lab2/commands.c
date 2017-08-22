// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// commands.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* Contains methods for interacting with the GPIOs
 * Contains methods for LCD display commands for LCD interaction
 */

// Includes the necessary libraries for the functions used
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// Changes the output value of a gpio pin
// Takes in int val and writes val to the file FILE *pin
void gpioout(FILE *pin, int val) {
  fprintf(pin, "%d", val);
  fflush(pin);
}

// Changes the direction of a gpio pin
// Takes in char *dir and writes the string to the file FILE *pin
void gpiodir(FILE *pin, char *dir) {
  fprintf(pin, "%s", dir);
  fflush(pin);
}

// Creates a gpio device file by writing int pnum to the file export
void gpioexport(FILE *export, int pnum) {
  // Sends pnum to export and creates gpio value for that pin
  fprintf(export, "%d", pnum);
  fflush(export);

}

// Accesses the gpio direction file for the GPIO at int pnum
// Returns the file pointer pointing to this direction file
FILE * createdir(int pnum) {
  FILE * dir;
  char dirpath[40] = "/sys/class/gpio/gpio";
  char buffer[5];

  // Creates pathname for direction file
  sprintf(buffer, "%d", pnum);
  strncat(dirpath, buffer, 40);
  strncat(dirpath, "/direction", 40);
  
  // Accesses the direction file of the created pin
  dir = fopen(dirpath, "w");
  // Exits program if the direction file cannot be accessed
  if (dir == NULL) {
    printf("Error: failed to open direction for gpio%d\n", pnum);
    exit(0);
  }
  fseek(dir, 0, SEEK_SET);
  gpiodir(dir, "out");  // Default direction is out
  return dir;
}

// Accesses the gpio value file for the GPIO at int pnum
// Returns the file pointer pointing to this value file
FILE * createval(int pnum) {
  FILE * val;
  // Creates pathname for value file
  char valpath[40] = "/sys/class/gpio/gpio";
  char buffer[5];
  sprintf(buffer, "%d", pnum);
  strncat(valpath, buffer, 40);
  strncat(valpath, "/value", 40);
  
  // Accesses the value file of the created pin
  val = fopen(valpath, "w");
  // Exits program if the value file cannot be accessed
  if (val == NULL) {
    printf("Error: failed to open value for gpio%d\n", pnum);
    exit(0);
  }
  fseek(val, 0, SEEK_SET);
  gpioout(val, 0);  // Default value is 0
  return val;
}

// Flips the enable pin (from input FILE *pin) to 1 for 1000 us,
// then turns the enable to 0 again (pulse of 1000 us)
void enable(FILE *pin) {
  gpioout(pin, 1);
  usleep(1000);
  gpioout(pin, 0);
}

// Changes the sel (register select) and rw (read/write) 
// If choose is 0, then sel = 0 and rw = 0 and LCD is in instruction mode
// If choose is 2, then sel = 1 and rw = 0 and LCD is in character write mode
void instr(FILE *sel, FILE *rw, int choose) {
  if (choose == 0) {
    gpioout(sel, 0);
    gpioout(rw, 0);
  } else if (choose == 2) {
    gpioout(sel, 1);
    gpioout(rw, 0);
  }
}

// Writes a character to the LCD display
// Sends the binary in *out input to the *val output to the LCD display
void writechar(FILE *sel, FILE *rw, FILE *en, FILE *val[8], int *out) {
  instr(sel, rw, 2);  // Character write mode
  gpioout(val[0], out[0]);
  gpioout(val[1], out[1]);
  gpioout(val[2], out[2]);
  gpioout(val[3], out[3]);
  gpioout(val[4], out[4]);
  gpioout(val[5], out[5]);
  gpioout(val[6], out[6]);
  gpioout(val[7], out[7]);
  enable(en);
  usleep(250000);  // Pause so the user can see each character being written
}

// Shifts the display screen to the left
void leftshift(FILE *sel, FILE *rw, FILE *en, FILE *val[8]) {
  instr(sel, rw, 0);
  gpioout(val[0], 0);
  gpioout(val[1], 0);
  gpioout(val[2], 0);
  gpioout(val[3], 1);
  gpioout(val[4], 1);
  gpioout(val[5], 0);
  gpioout(val[6], 0);
  gpioout(val[7], 0);
  enable(en);
}

// Returns the LCD's cursor to the start of the display, and shifts the
// entire display back to the front
void rtnhome(FILE *sel, FILE *rw, FILE *en, FILE *val[8]) {
  instr(sel, rw, 0);
  gpioout(val[0], 0);
  gpioout(val[1], 1);
  gpioout(val[2], 0);
  gpioout(val[3], 0);
  gpioout(val[4], 0);
  gpioout(val[5], 0);
  gpioout(val[6], 0);
  gpioout(val[7], 0);
  enable(en);
  usleep(20000);
}

// Moves the cursor to the beginning of the second line
void sndline(FILE *sel, FILE *rw, FILE *en, FILE *val[8]) {
  instr(sel, rw, 0);
  gpioout(val[0], 0);
  gpioout(val[1], 0);
  gpioout(val[2], 0);
  gpioout(val[3], 0);
  gpioout(val[4], 0);
  gpioout(val[5], 0);
  gpioout(val[6], 1);
  gpioout(val[7], 1);
  enable(en);
}

// Clears the display of the LCD and returns the cursor to the front of the
// display. Also shifts the display back to the beginning 
void clrdisplay(FILE *sel, FILE *rw, FILE *en, FILE *val[8]) {
  instr(sel, rw, 0);
  gpioout(val[0], 1);
  gpioout(val[1], 0);
  gpioout(val[2], 0);
  gpioout(val[3], 0);
  gpioout(val[4], 0);
  gpioout(val[5], 0);
  gpioout(val[6], 0);
  gpioout(val[7], 0);
  enable(en);
  usleep(20000);
}

// Initializes the LCD display upon startup
void initialize(FILE *sel, FILE *rw, FILE *en, FILE *val[8]) {
  // Waits for more than 15ms after the display is powered
  usleep(20000);
  // Executes a Function Set Command
  gpioout(val[4], 1);
  gpioout(val[5], 1);
  enable(en);
  // Wait for more than 4.1ms
  usleep(5000);
  // Executes a Function Set Command
  gpioout(val[4], 1);
  gpioout(val[5], 1);
  enable(en);
  // Wait for more than 100 us
  usleep(200);
  // Exectues a Function Set Command
  gpioout(val[4], 1);
  gpioout(val[5], 1);
  enable(en);
  // Function Set to 8 bit display and 2 lines of font
  gpioout(val[2], 1);
  gpioout(val[3], 1);
  enable(en);
  // Turns the display off
  gpioout(val[2], 0);
  gpioout(val[4], 0);
  gpioout(val[5], 0);
  enable(en);
  // Clear the display
  gpioout(val[0], 1);
  gpioout(val[3], 0);
  enable(en);
  // Entry Mode Set to increment 1 and display shift operation
  gpioout(val[2], 1);
  gpioout(val[1], 1);
  enable(en);
  // Turns the display one
  gpioout(val[3], 1);
  enable(en);
  // Short delay
  usleep(100);
}
