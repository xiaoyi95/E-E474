// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// receiver.c
// CSE/EE 474 Lab 4
// 5/20/2017

/* receiver.c will receive a signal from sender.c if the distance
 * sensor detects that an object is close to it.
 * This is achieved using a timer interrupt. Each sensor has a
 * different signal number to distinguish which side detected an
 * object.
 */

// Includes the libraries used
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Defines the signal numbers for each sensor
#define SIG_FRONT 20
#define SIG_BACK 30
#define SIG_LEFT 40
#define SIG_RIGHT 50

// Checks which signal was received and prints it to the screen
void sig_handler(int signo) {
  if (signo == 20) {
    printf("received front\n");
  } else if (signo == 30) {
    printf("received back\n");
  } else if (signo == 40) {
    printf("received left\n");
  } else if (signo == 50) {
    printf("received right\n");
  }
}

int main(void) {
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

  // Keeps the program running. Will be interrupted if the sensor
  // detects something.
  while(1) {
    usleep(10000000);  // sleep to save CPU power
  }
  
  return 0;
}
