// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// basicreq.c
// CSE/EE 474 Lab 5
// 6/3/2017

/* basicreq.c tests the basic functionality of accepting input from
 * from a bluetooth, and using that input to drive the tank.
 * This program will use the h-bridge driver to drive the tank forwards
 * if it receives a w, backwards if it receives an a, left if is receives
 * an a, right if it receives a d, and brake if is receives a space " ".
 */

// Includes the libraries used
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B9600  // Defines the baudrate for the bluetooth
#define MODEMDEVICE "/dev/ttyO4"  // Defines the pathname for the uart

// Defines the GPIO pin values for motor A 1&2, and motor B 1&2
#define AIN2 66
#define AIN1 69
#define BIN1 47
#define BIN2 27

// The output pins for the motors are accessible by all functions
FILE *ain2, *ain1, *bin1, *bin2;

// Function declaration for motor control
void gpioexport(int pin, FILE **val);
void gpioout(FILE *pin, int out);
void forward();
void reverse();
void brake();
void rightturn();
void leftturn();
void rightpivot();
void leftpivot();

int main() {
  FILE *slots, *dutyA, *dutyB, *perA, *perB;  // Files for accessing the pwm

  // Initialize the GPIOs for A1, A2, B1, and B2
  gpioexport(AIN2, &ain2);
  gpioexport(AIN1, &ain1);
  gpioexport(BIN1, &bin1);
  gpioexport(BIN2, &bin2);

  // Open the slots file for initalizing the PWM modules
  slots = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots == NULL) {
    printf("Error: failed to open pwm slots\n");
    return -1;
  }

  // Enable both PWM_1A and PWM_2A
  fprintf(slots, "%s", "am33xx_pwm");
  fflush(slots);
  fprintf(slots, "%s", "bone_pwm_P8_19");
  fflush(slots);
  fprintf(slots, "%s", "bone_pwm_P9_14");
  fflush(slots);

  usleep(5000000);

  // Opens the duty and period files for the pwm connected to motor A
  dutyA = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/duty", "w");
  if (dutyA == NULL) {
    printf("Error: failed to open pwma duty\n");
    return -1;
  }
  perA = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/period", "w");
  if (perA == NULL) {
    printf("Error: failed to open pwma period\n");
    return -1;
  }
  // Opens the duty and period files for the pwm connected to motor B
  dutyB = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/duty", "w");
  if (dutyB == NULL) {
    printf("Error: failed to open pwmb duty\n");
    return -1;
  }
  perB = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/period", "w");
  if (perB == NULL) {
    printf("Error: failed to open pwmb period\n");
    return -1;
  }
  
  // Left wheel speed, sets and starts the PWM
  fprintf(perA, "%d", 1000000);
  fflush(perA);
  fprintf(dutyA, "%d", 1000000/5);
  fflush(dutyA);
  // Right wheel speed, sets and starts the PWM
  fprintf(perB, "%d", 1000000);
  fflush(perB);
  fprintf(dutyB, "%d", 1000000/5);
  fflush(dutyB);

  brake();  // Car starts in brake on default

  int fd, res;  // Files numbers for open and read
  struct termios newtio;  // Struct for bluetooth settings
  char buf[255];  // Buffer for bluetooth input
  // Sets up the uart file and opens it for reading/writing
  system("echo uart4 > /sys/devices/bone_capemgr.9/slots");
  fd = open(MODEMDEVICE, O_RDWR);
  if (fd < 0) {
    printf("Error opening file for usrt");
    return -1;
  }
  printf("%d\n", fd);

  // Defines the settings flags for the bluetooth
  // BAUDRATE: sets the baud rate; CS8: 8-bit, no parity;
  // CLOCAL: local connection; CREAD: enable receiving characters
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;  // Ignore parity errors
  newtio.c_oflag = 0;       // Gets raw output
  newtio.c_lflag = ICANON;  // Disable echo, enable canonical input
  
  tcflush(fd,TCIFLUSH);           // Flushes the uart file
  tcsetattr(fd,TCSANOW,&newtio);  // Sets the terminal settings, change occurs immediately
  
  while (1) {
    // Reads input from the bluetooth
    res = read(fd, buf, 255);
    buf[res]='\0';
    // Reads the first inputted character in the buffer only
    if (res > 0) {
      printf("%s", buf);
      // If the character is w, drive forward
      if (buf[0] == 'w') {
        forward();
        // If the character is s, drive backward
      } else if (buf [0] == 's') {
        reverse();
        // If the character is a, turn left
      } else if (buf [0] == 'a') {
        leftturn();
        // If the character is d, turn right
      } else if (buf [0] == 'd') {
        rightturn();
        // If the character is a space, brake 
      } else if (buf [0] == ' ') {
        brake();
      }
    }
  }

  // Closes all of the open files
  close(fd);
  fclose(slots);
  fclose(perA);
  fclose(perB);
  fclose(dutyA);
  fclose(dutyB);
  fclose(ain1);
  fclose(ain2);
  fclose(bin1);
  fclose(bin2);
  return 0;
}

// Exports the GPIO requested by input int pin, and sets its default values
// to output and value=0. Returns the opened value file to the input
// pointer val, so the user can control the value.
void gpioexport(int pin, FILE **val) {
  FILE *exp, *dir;  // Export and direction files

  // Opens the export file for the GPIOs
  exp = fopen("/sys/class/gpio/export", "w");
  if (exp == NULL) {
    printf("Error: Failed to open gpio export file\n");
    exit(1);
  }

  // Exports the pin number (requests the GPIO of this pin number)
  fprintf(exp, "%d", pin);
  fflush(exp);

  // Opens the direction file for the pin
  char dirBuf[40];
  sprintf(dirBuf, "/sys/class/gpio/gpio%d/direction", pin);
  dir = fopen(dirBuf, "w");
  if (dir == NULL) {
    printf("Error: Failed to open gpio%d direction file\n", pin);
    exit(1);
  }
  // Sets the default value to out
  fprintf(dir, "%s", "out");
  fflush(dir);

  // Opens the value file for the pin
  char valBuf[40];
  sprintf(valBuf, "/sys/class/gpio/gpio%d/value", pin);
  *val = fopen(valBuf, "w");
  if (*val == NULL) {
    printf("Error: Failed to open gpio%d value file\n", pin);
    exit(1);
  }
  // Sets the default value to 0 (low)
  fprintf(*val, "%d", 0);
  fflush(*val);

  // Closes the export and direction files
  fclose(exp);
  fclose(dir);
}

// Changes the GPIO value inputted to the value indicated by int out
void gpioout(FILE *pin, int out) {
  fprintf(pin, "%d", out);
  fflush(pin);
}

// Sets the motor to spin both wheels forwards
void forward() {
  gpioout(ain1, 0);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 0);
}

// Sets the motor to spin both wheels backwards
void reverse() {
  gpioout(ain1, 1);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 1);
}

// Sets the motor to stop both wheels from moving
void brake() {
  gpioout(ain1, 1);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 1);
}

// Sets the motor to spin only the left wheel forward
void rightturn() {
  gpioout(ain1, 0);
  gpioout(ain2, 0);
  gpioout(bin1, 1);
  gpioout(bin2, 0);
}

// Sets the motor to spin only the right wheel forward
void leftturn() {
  gpioout(ain1, 0);
  gpioout(ain2, 1);
  gpioout(bin1, 0);
  gpioout(bin2, 0);
}

// Sets the motor to spin the left wheel forward and right wheel backward
void rightpivot() {
  gpioout(ain1, 1);
  gpioout(ain2, 0);
  gpioout(bin1, 1);
  gpioout(bin2, 0);
}

// Sets the motor to spin the right wheel forward and left wheel backward
void leftpivot() {
  gpioout(ain1, 0);
  gpioout(ain2, 1);
  gpioout(bin1, 0);
  gpioout(bin2, 1);
}
