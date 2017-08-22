// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// drive.c
// CSE/EE 474 Lab 4
// 5/20/2017

/* drive.c controls the h-bridge driver to drive the car, for the autodriving
 * function. The default movement is for the tank to move forward. When the 
 * program receives an interrupt for the front sensor, the tank will then reverse.
 * If the tank is reversing and it receives a back sensor signal, then it will
 * move forwards again. Additionally, there are LEDs that will blink when either
 * the front, back, right, or left sensors are detecting something.
 * 
 * You may notice that there are right and left sensor detection, as well as right
 * and left turn functions. The autodrive for turning was commented out as the
 * tanks's motor and wheels are faulty (the spinning on one of the wheels could not 
 * be controlled well despite changing PWM values) and the tank was not turning
 * reliably, neither for pivoting or slow turning. 
 */

// Includes the libraries used
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Defines the signal numbers for each sensor
#define SIG_FRONT 20
#define SIG_BACK 30
#define SIG_LEFT 40
#define SIG_RIGHT 50

// Defintes the GPIO pin values for motor A and motor B
#define AIN2 66
#define AIN1 69
#define BIN1 47
#define BIN2 27

// Defines the GPIO pin values for the LEDs
#define LED_RIGHT 67
#define LED_BACK 68
#define LED_FRONT 44
#define LED_LEFT 26

// The output pins for the motors and leds
FILE *ain2, *ain1, *stby, *bin1, *bin2, *ledr, *ledb, *ledf, *ledl;
// The value for the LEDs
int ftrue = 0;
int btrue = 0;
int rtrue = 0;
int ltrue = 0;

// Function declaration for gpio and motor control
void gpioexport(int pin, FILE **val);
void gpioout(FILE *pin, int out);
void forward();
void reverse();
void brake();
void rightturn();
void leftturn();
f(*val, "%d", 0);

// Checks which signal was received and prints it to the screen
void sig_handler(int signo)
{
  if (signo == 20) {
    ftrue = 1;
  } else if (signo == 30) {
    btrue = 1;
  } else if (signo == 40) {
    ltrue = 1;
  } else if (signo == 50) {
    rtrue = 1;
  }
}

int main(void) {
  FILE *slots, *dutyA, *dutyB, *perA, *perB;  // Files for accessing the PWM

  // Initialize the GPIOs for A1, A2, B1, and B2
  gpioexport(AIN2, &ain2);
  gpioexport(AIN1, &ain1);
  gpioexport(BIN1, &bin1);
  gpioexport(BIN2, &bin2);

  // Initialize the GPIOs for the LEDs
  gpioexport(LED_RIGHT, &ledr);
  gpioexport(LED_FRONT, &ledf);
  gpioexport(LED_BACK, &ledb);
  gpioexport(LED_LEFT, &ledl);

  // Open the slots file for initializing the PWM modules
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

  usleep(2000000);

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
  fprintf(perA, "%d", 5000000);
  fflush(perA);
  fprintf(dutyA, "%d", 5000000/4);
  fflush(dutyA);
  // Right wheel speed, sets and starts the PWM
  fprintf(perB, "%d", 5000000);
  fflush(perB);
  fprintf(dutyB, "%d", 5000000/2);
  fflush(dutyB);

  // Default set to move forwards
  forward();

  // Checks for the signals for each sensor, prints an error if
  // we fail to receive the signal
  if (signal(SIG_FRONT, sig_handler) == SIG_ERR)
  	printf("\ncan't catch SIGINT\n");
  if (signal(SIG_BACK, sig_handler) == SIG_ERR)
  	printf("\ncan't catch SIGINT\n");
  if (signal(SIG_LEFT, sig_handler) == SIG_ERR)
      printf("\ncan't catch SIGINT\n");
  if (signal(SIG_RIGHT, sig_handler) == SIG_ERR)
  	printf("\ncan't catch SIGINT\n");

  // Continuously runs. Will be interrupted if the sensor detects something.
  while(1) {
    // Sets the value of the LEDs
    gpioout(ledf, ftrue);
    gpioout(ledb, btrue);
    gpioout(ledr, rtrue);
    gpioout(ledl, ltrue);

    // If something is detected in the front, then the tank will reverse.
    // If something is detected in the back, then the tank will move forwards.
    if (ftrue == 1 && btrue == 0) {
      reverse();
    } else if (btrue == 1 && ftrue == 0) {
      forward();
    } /*else if (rtrue == 1 && ltrue == 0) {
			leftturn();
		}	else if (ltrue == 1 && rtrue == 0) {
			rightturn();
		}*/

    usleep(10000); // sleep so the LED blink can actually be observed
    ftrue = 0;
    btrue = 0;
    rtrue = 0;
    ltrue = 0;
  }

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
