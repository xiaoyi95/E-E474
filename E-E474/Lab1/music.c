// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// music.c
// CSE/EE 474 Lab 1
// 4/7/2017

/* This file configures three GPIO pins that output to LEDs. The LEDs are lit
 * counting from 0-7 in binary. Every time the number changes, a PWM pin
 * outputting to a piano buzzer plays a different note.
 */

#include <stdio.h>     // for File I/O and printf
#include <time.h>      // for usleep

#define GPIO_PIN0 66   // GPIO pin number
#define GPIO_PIN1 69   // GPIO pin number
#define GPIO_PIN2 45   // GPIO pin number

#define REPEAT 3       // Sets the number of full 0-7 cycles for the counter
#define VOLUME 100     // The default value 100 sets the volume

int main() {
  // Files to open in the beaglebone for the gpio/pwm outputs
  FILE *sys, *dir0, *dir1, *dir2, *val0, *val1, *val2, *note, *per, *duty, *run;

  // Opens the export file for the gpio pins, checks if the file opens properly
  sys = fopen("/sys/class/gpio/export", "w");
  if (sys == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }
  
  // Creates the device folder for the GPIO pins to be used
  fseek(sys, 0, SEEK_SET);
  fprintf(sys, "%d", GPIO_PIN0);
  fflush(sys);
  fprintf(sys, "%d", GPIO_PIN1);
  fflush(sys);
  fprintf(sys, "%d", GPIO_PIN2);
  fflush(sys);

  // Opens the file that controls if the GPIO pins are detecting input or output
  dir0 = fopen("/sys/class/gpio/gpio66/direction", "w");
  dir1 = fopen("/sys/class/gpio/gpio69/direction", "w");
  dir2 = fopen("/sys/class/gpio/gpio45/direction", "w");
  // Checks if the direction files opened properly, else exits program
  if  (dir0 == NULL) {
    printf("Error: Failed to open gpio66 direction file\n");
    return 1;
  } else if (dir1 == NULL) {
    printf("Error: Failed to open gpio69 direction file\n");
    return 1;
  } else if (dir2 == NULL) {
    printf("Error: Failed to open gpio45 direction file\n");
    return 1;
  }
  
  // Sets the buffer at the beginning of the files
  fseek(dir0, 0, SEEK_SET);
  fseek(dir1, 0, SEEK_SET);
  fseek(dir2, 0, SEEK_SET);

  // Sets the direction of all GPIO pins to output
  fprintf(dir0, "%s", "out");
  fflush(dir0);
  fprintf(dir1, "%s", "out");
  fflush(dir1);
  fprintf(dir2, "%s", "out");
  fflush(dir2);

  //Opens the file that controls if the GPIO pins are  high or low
  val0 = fopen("/sys/class/gpio/gpio66/value", "w");
  val1 = fopen("/sys/class/gpio/gpio69/value", "w");
  val2 = fopen("/sys/class/gpio/gpio45/value", "w");
  // Checks if the value files opened properly, else exits program
  if  (val0 == NULL) {
    printf("Error: Failed to open gpio66 value file\n");
    return 1;
  } else if (val1 == NULL) {
    printf("Error: Failed to open gpio69 value file\n");
    return 1;
  } else if (val2 == NULL) {
    printf("Error: Failed to open gpio45 value file\n");
    return 1;
  }
  
  // Sets the buffer at the beginning of the files
  fseek(val0, 0, SEEK_SET);
  fseek(val1, 0, SEEK_SET);
  fseek(val2, 0, SEEK_SET);

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
  
  // Sets the default values for the PWM output
  fprintf(per, "%d", 3822250);
  fflush(per);
  fprintf(duty, "%d", 1911125);
  fflush(duty);
  fprintf(run, "%d", 1);
  fflush(run);

  int count = 0;  // Sets the default value for the LED counter
  int time = 8 * REPEAT;  // Sets the time to match the number of repeats
  // Sets the pitches of the notes to be played by the buzzer (middle C scale)
  int pitch[8] = {3822250, 3405241, 3033723, 2863459, 2551053, 2272727, 2024771, 1911128};
  
  // Plays the C scale for a REPEAT number of times, outputting from 0-7 in 
  // binary to the GPIO outputs
  while(time > 0) {
    // Turns the LED representing binary value 4 on and off
    if (count == 4 || count == 5 || count == 6 || count == 7) {
      fprintf(val0, "%d", 1);
      fflush(val0);
    } else {
      fprintf(val0, "%d", 0);
      fflush(val0);
    }
    // Turns the LED representing the binary value 2 on and off
    if (count == 2 || count == 3 || count == 6 || count == 7) {
      fprintf(val1, "%d", 1);
      fflush(val1);
    } else {
      fprintf(val1, "%d", 0);
      fflush(val1);
    }
    // Turns the LED representing the binary value 1 on and off
    if (count == 1 || count == 3 || count == 5 || count == 7) {
      fprintf(val2, "%d", 1);
      fflush(val2);
    } else {
      fprintf(val2, "%d", 0);
      fflush(val2);
    }

    // Changes the period and duty cycle of the PWM according to the note
    // to be played at its respective counter number
    fprintf(per, "%d", pitch[count]);
    fflush(per);
    fprintf(duty, "%d", pitch[count]/VOLUME);
    fflush(duty);

    usleep(500000);  // Half second hold for each note

    if (count != 7) {
      count++;  // Increments the count
    } else {
      count = 0;  // Loops back to zero if the count is at 7
    }
    time--;
  }

  // Turns off all of the LEDs/GPIO outputs and the PWM/buzzer
  fprintf(val0, "%d", 0);
  fflush(val0);
  fprintf(val1, "%d", 0);
  fflush(val1);
  fprintf(val2, "%d", 0);
  fflush(val2);
  fprintf(run, "%d", 0);
  fflush(run);

  // Closes all files
  fclose(sys);
  fclose(dir0);
  fclose(dir1);
  fclose(dir2);
  fclose(val0);
  fclose(val1);
  fclose(val2);
  fclose(note);
  fclose(per);
  fclose(duty);
  fclose(run);
  return 0;
}

