// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// drive.c
// CSE/EE 474 Lab 5
// 6/3/2017

/* drive.c controls the h-bridge driver to drive the car. The default movement is
 * for the tank to brake. When the program receives an interrupt from the distance 
 * sensors, it will update the sensor values and output to the LEDs if the sensors
 * are detecting something. When the program receives an interrupt from the user,
 * if will go forward, reverse, turn right, turn left, or brake according to the
 * received signal, as long as there is no sensor detection in that direction.
 */

// Includes the libraries used
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Defines the signal numbers for the sensor and the direction inputs
#define SIG_DSENSE 15
#define SIG_FORWARD 20
#define SIG_REVERSE 25
#define SIG_LEFT 30
#define SIG_RIGHT 35
#define SIG_BRAKE 40

// Defines th GPIO pin values for motor A and motor B
#define AIN2 66
#define AIN1 69
#define BIN1 47
#define BIN2 27

// Defines the GPIO pin values for the LEDs
#define LED_RIGHT 67
#define LED_BACK 68
#define LED_FRONT 44
#define LED_LEFT 26

// The output pins for the motors and LEDs
FILE *ain2, *ain1, *bin1, *bin2;
FILE *ledr, *ledb, *ledf, *ledl, *shared;

// Pathname of the pipe to be created
char * path= "/root/pipe";
// Current readings from the sensors (1 for object close, 0 if not)
char curr[5] = {'0', '0', '0', '0', '\0'};
// Indicates what mode the tank is currently driving in
int status = 0;

// Function declaration for gpio and motor control
void gpioexport(int pin, FILE **val);
void gpioout(FILE *pin, int out);
void forward();
void reverse();
void brake();
void rightturn();
void leftturn();
void rightpivot();
void leftpivot();

// Checks which signal was received and acts accordingly depending on the sensor
// readings and movement of the tank
void sig_handler(int signo) {
  // If the signal is from the sensor
  if (signo == SIG_DSENSE) {
    // Opens the shared pipe
    shared = fopen(path, "r");
    if (shared == NULL) {
      printf("fopen receiver side failed\n");
      exit(1);
    }
    // Reads the current sensor readings from the pipe
    fscanf(shared, "%s", curr);    
    fclose(shared);
    
    // Updates the LED output to reflect the current sensor reading
    gpioout(ledf, curr[0] - '0');
    gpioout(ledb, curr[1] - '0');
    gpioout(ledr, curr[2] - '0');
    gpioout(ledl, curr[3] - '0');
    
    // If the tank is moving in a direction of a sensor that has just turned on,
    // send the tank into brake mode (so it doesn't hit anything)
    if ((curr[0] - '0' == 1 && status == 1) || (curr[1] - '0' == 1 && status == 2) ||
        (curr[2] - '0' == 1 && status == 3) || (curr[3] - '0' == 1 && status == 4)) {
      brake();
    }
  // If the forward signal is received and the forward sensor is off, go forward
  } else if (signo == SIG_FORWARD) {
    if (curr[0] - '0' == 0) {
      forward();
    }
  // If the reverse signal is received and the reverse sensor is off, reverse
  } else if (signo == SIG_REVERSE) {
    if (curr[1] - '0' == 0) {
      reverse();
    }
  // If the right signal is received and the right sensor is off, turn right
  } else if (signo == SIG_RIGHT) {
    if (curr[2] - '0' == 0) {
      rightturn();
    }
  // If the left signal is received and the left sensor is off, turn left
  } else if (signo == SIG_LEFT) {
     if (curr[3] - '0' == 0) {
      leftturn();
    }
  // If the brake signal is received, brake
  } else if (signo == SIG_BRAKE) {
    brake();
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

   // Left wheel speed
  fprintf(perA, "%d", 2000000);
  fflush(perA);
  fprintf(dutyA, "%d", 2000000/4);
  fflush(dutyA);
  // Right wheel speed
  fprintf(perB, "%d", 2000000);
  fflush(perB);
  fprintf(dutyB, "%d", 2000000/4);
  fflush(dutyB);
  
  // Creates the pipe for receiving sensor data
  if (access(path, F_OK) != 0) {
    if (mkfifo(path, 0666) != 0) {
      printf("mkfifo command failed\n");
      return -1;
    }
  }
  
  // Brake as the default setting
  brake();
  
  // Checks for the signals for each sensor, prints an error if
  // we fail to receive the signal
  if (signal(SIG_DSENSE, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIG_BRAKE, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIG_FORWARD, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIG_REVERSE, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIG_RIGHT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIG_LEFT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
    
  while (1) {
    usleep(5000000);  // Sleep to save CPU power
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
  status = 1;
}

// Sets the motor to spin both wheels backwards
void reverse() {
  gpioout(ain1, 0);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 0);
  status = 2;
}

// Sets the motor to stop both wheels from moving
void brake() {
  gpioout(ain1, 1);
  gpioout(ain2, 1);
  gpioout(bin1, 1);
  gpioout(bin2, 1);
  status = 0;
}

// Sets the motor to spin only the left wheel forward
void rightturn() {
  gpioout(ain1, 1);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 0);
  status = 3;
}

// Sets the motor to spin only the right wheel forward
void leftturn() {
  gpioout(ain1, 0);
  gpioout(ain2, 0);
  gpioout(bin1, 0);
  gpioout(bin2, 1);
  status = 4;
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
