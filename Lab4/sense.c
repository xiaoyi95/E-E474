// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// sense.c
// CSE/EE 474 Lab 4
// 5/20/2017

/* sense.c utilizes a timer interrupt process to send signals to receiver.c
 * 
 * For accurate readings, the sensor values should be averaged before using the
 * using the value to compare to the threshold value. The timing for the sensor 
 * samples can be configured using the following:
 *  FREQUENCY - frequency of the average sample value
 *  SAMPLE_RATE - number of samples to take before averaging 
 *
 * If the average sample value is greater than the threshold value, aka the
 * object is too close to the sensor, then sense.c will interrupt drive.c by
 * sending it a signal corresponding to one of the sensors.
 */

// Includes the libraries used
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>    
#include <string.h>     
#include <fcntl.h> 

#define FREQUENCY 100  // Frequency of the averaged samples
#define SAMPLE_RATE 100  // Number of samples to average
#define DETECT 900  // Detection value, when something is too close to a sensor

// Defines the signal numbers for each sensor
#define SIG_FRONT 20
#define SIG_BACK 30
#define SIG_LEFT 40
#define SIG_RIGHT 50

// ADC pin number for each of the sensor inputs
#define AIN_FRONT 0
#define AIN_BACK 1
#define AIN_RIGHT 2
#define AIN_LEFT 3

int count;  // Sample count
int front_sum, back_sum, right_sum, left_sum;  // Current sample sum

struct sigaction sa;
struct itimerval timer;
int pid;  // process id of the receiver

// Function declarations
void timer_Init();
void ADC_read(int pin, int *val);

// timer interrupt handler
void timer_handler(int whatever) {
  count++;  // Increment sample number
  int df, db, dr, dl;  // Value of the ADC reads

  // Read the value for each sensor
  ADC_read(AIN_FRONT, &df);
  ADC_read(AIN_BACK, &db);
  ADC_read(AIN_RIGHT, &dr);
  ADC_read(AIN_LEFT, &dl);

  // Add this to the total sum
  front_sum += df;
  back_sum += db;
  left_sum += dl;
  right_sum += dr;

  // If we have sampled enough values, then decide whether a signal
  // needs to be sent to the user
  if (count == SAMPLE_RATE) {
    // If the average of the front samples is greater than the threshold
    // value, then send a signal to the receiver for front detection
    if(front_sum / SAMPLE_RATE > DETECT) {
      if(kill(pid,SIG_FRONT) != 0){ 
	printf("Can't send msg\n");
	exit(0);
      }
      printf("Signal for front sent\n");
    }
    // If the average of the back samples is greater than the threshold
    // value, then send a signal to the receiver for back detection
    if(back_sum / SAMPLE_RATE > DETECT) {
      if(kill(pid,SIG_BACK) != 0){ 
	printf("Can't send msg\n");
	exit(0);
      }
      printf("Signal for back sent\n");
    }
    // If the average of the left samples is greater than the threshold
    // value, then send a signal to the receiver for left detection
    if(left_sum / SAMPLE_RATE > DETECT) {
      if(kill(pid,SIG_LEFT) != 0){ 
	printf("Can't send msg\n");
	exit(0);
      }
      printf("Signal for left sent\n");
    }
    // If the average of the right samples is greater than the threshold
    // value, then send a signal to the receiver for right detection
    if(right_sum / SAMPLE_RATE > DETECT) {
      if(kill(pid,SIG_RIGHT) != 0){ 
	printf("Can't send msg\n");
	exit(0);
      }
      printf("Signal for right sent\n");
    }

    // Set the count and sum values back to 0
    count = 0;
    front_sum = 0;
    back_sum=0;
    right_sum=0;
    left_sum=0;
  }
}

int main(int argc, char *argv[]) {	
  // Prints the input arguments
  int count;
  printf ("This program was called with \"%s\".\n",argv[0]);
  if (argc > 1) {
    for (count = 1; count < argc; count++) {
      printf("argv[%d] = %s\n", count, argv[count]);
    }
  } else {  // Exits if the program does not receive the proper arguments
    printf("The command needs more arguments.\n"); 
    return -1; 
  }

  // Opens the slots file for using the ADC pins
  FILE *slots;
  slots = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots == NULL) {
    printf("Error: Failed to open export\n");
    return 1;
  }

  // Enables the ADC pins for taking input
  fseek(slots, 0, SEEK_SET);
  fprintf(slots, "cape-bone-iio");
  fflush(slots);
  fclose(slots);

  // Starts the timer
  pid = atoi(argv[1]);
  timer_Init();
  while(1);

  return 0;
}

// Initializes the timer for the timer for sampling the ADC values
void timer_Init() {
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = &timer_handler;
  sigaction (SIGVTALRM, &sa, NULL);

  // Configure the timer to expire after a calculated frequency time
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 1000000 / (FREQUENCY * SAMPLE_RATE);
  // and every calculated frequency time after that
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 1000000 / (FREQUENCY * SAMPLE_RATE);
  // Start a virtual timer. It counts down whenever this process is executing
  setitimer (ITIMER_VIRTUAL, &timer, NULL);
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
