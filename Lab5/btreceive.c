// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// btreceive.c
// CSE/EE 474 Lab 5
// 6/3/2017

/* btreceive.c opens the uart file connected to the bluetooth and receives 
 * input from the uart. The program will read the information when the user
 * in the terminal has entered a newline character. If the program receives
 * w, a, s, d, or space character, it will send the corresponding character
 * to the drive program.
 *
 * Note that the program can also send signal to the temperature program to
 * decide the temperature display. Because the bluetooth was being finicky
 * with the connection after the system crash we incurred before demo, we
 * decided to edit this out and let the sensor default to Fahrenheit.
 * (Letting the user pick requires a system restart, which would require
 * us to reconnect to the Bluetooth. We didn't want to risk disconnecting
 * and having to try to reconnect before we miss the time window for the
 * temperature selection).
 */

// Includes the libraries used
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Defines the signal numbers for each letter
#define SIG_W 20
#define SIG_S 25
#define SIG_A 30
#define SIG_D 35
#define SIG_SPACE 40

// Defines the signal numbers for the temperature selection
#define SIG_K 50
#define SIG_C 55
#define SIG_F 60

#define BAUDRATE B9600            // Baudrate for the bluetooth/uart
#define MODEMDEVICE "/dev/ttyO4"  // Pathname for the uart

int pid1;  // Process ID for the drive program
int pid2;  // Process ID for the sensor program

int main(int argc, char*argv[]) {
  int count;
  printf ("This program was called with \"%s\".\n",argv[0]);

  // Checks if the program got arguments, prints arguments to the screen
  if (argc > 1) {
    for (count = 1; count < argc; count++) {
      printf("argv[%d] = %s\n", count, argv[count]);
    }
    // Error if no process ID received
  } else {
    printf("The command needs more arguments.\n"); 
    return -1; 
  }
  // Saves the first argument as the process ID
  pid1 = atoi(argv[1]);
  //pid2 = atoi(argv[2]);

  int fd, res;  // File number of open, read error check
  struct termios newtio;  // struct for bluetooth settings
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
    // Uses the first inputted character in the buffer
    if (res > 0) {
      printf("%s", buf);
      // If the character is w, send the signal for w to the drive program
      if (buf[0] == 'w') {
        if(kill(pid1,SIG_W) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is s, send the signal for s to the drive program
      } else if (buf [0] == 's') {
        if(kill(pid1,SIG_S) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is a, send the signal for a to the drive program
      } else if (buf [0] == 'a') {
        if(kill(pid1,SIG_A) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is d, send the signal for d to the drive program
      } else if (buf [0] == 'd') {
        if(kill(pid1,SIG_D) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is space, send the signal for space to the drive program
      } else if (buf [0] == ' ') {
        if(kill(pid1,SIG_SPACE) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      }
      // If the character is k, send the signal for space to the sensor program
      /* else if (buf [0] == 'k') {
        if(kill(pid2,SIG_K) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is c, send the signal for space to the sensor program
      } else if (buf [0] == 'c') {
        if(kill(pid2,SIG_C) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      // If the character is f, send the signal for space to the sensor program
      } else if (buf [0] == 'f') {
        if(kill(pid2,SIG_F) != 0) { 
          printf("Can't send msg\n");
          return -1;
        }
      }*/
    }
  }
}
