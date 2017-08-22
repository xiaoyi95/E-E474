// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// motor_test.c
// CSE/EE 474 Lab 4
// 5/20/2017

/* motor_test.c tests the functionality of the h-bridge driver.
 * This program will set the driver so that the car moves forward
 * for 3 seconds, reverses for 3 seconds, pivots right for 3 seconds,
 * pivots left for 3 seconds, then brakes.
 */

// Includes the libraries used
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Defines the GPIO pin values for motor A 1&2, and motor B 1&2
#define AIN2 66
#define AIN1 69
#define BIN1 47
#define BIN2 27

// The output pins for the motors are accessible by all functions
FILE *ain2, *ain1, *bin1, *bin2;

// Function declaration for gpio and motor control
void gpioexport(int pin, FILE **val);
void gpioout(FILE *pin, int out);
void forward();
void reverse();
void brake();
void rightturn();
void leftturn();

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
    return 1;
  }

  // Enable both PWM_1A and PWM_2A
  fprintf(slots, "%s", "am33xx_pwm");
  fflush(slots);
  fprintf(slots, "%s", "bone_pwm_P8_19");
  fflush(slots);
  fprintf(slots, "%s", "bone_pwm_P9_14");
  fflush(slots);

  usleep(1000000);

  // Opens the duty and period files for the pwm connected to motor A
  dutyA = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/duty", "w");
  if (dutyA == NULL) {
    printf("Error: failed to open pwma duty\n");
    return 1;
  }
  perA = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/period", "w");
  if (perA == NULL) {
    printf("Error: failed to open pwma period\n");
    return 1;
  }
  // Opens the duty and period files for the pwm connected to motor B
  dutyB = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/duty", "w");
  if (dutyB == NULL) {
    printf("Error: failed to open pwmb duty\n");
    return 1;
  }
  perB = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/period", "w");
  if (perB == NULL) {
    printf("Error: failed to open pwmb period\n");
    return 1;
  }
  
  // Left wheel speed, sets and starts the PWM
  fprintf(perA, "%d", 2000000);
  fflush(perA);
  fprintf(dutyA, "%d", 2000000/4);
  fflush(dutyA);
  // Right wheel speed, sets and starts the PWM
  fprintf(perB, "%d", 2000000);
  fflush(perB);
  fprintf(dutyB, "%d", 2000000/2);
  fflush(dutyB);

  // Controls the motor to move the car forward, backwards,
  // and one wheel at a time
  forward();
  usleep(3000000);
  reverse();
  usleep(3000000);
  rightturn();
  usleep(3000000);
  leftturn();
  usleep(3000000);  
  brake();

  // Closes all of the open files
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
  gpioout(ain1, 1);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 1);
}

// Sets the motor to spin both wheels backwards
void reverse() {
  gpioout(ain1, 0);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 0);
}

// Sets the motor to stop both wheels from moving
void brake() {
  gpioout(ain1, 1);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 1);
}

// Sets the motor to spin only the left wheel forward
// (If the wheels/motor were functional, the car would pivot right)
void rightturn() {
  gpioout(ain1, 1);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 0);
}

// Sets the motor to spin only the right wheel forward
// (If the wheels/motor were functional, the car would pivot left)
void leftturn() {
  gpioout(ain1, 0);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 1);
}
