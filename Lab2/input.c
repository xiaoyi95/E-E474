// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// input.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* This program takes user input and sends it to the LCD through
 * a pipe provided by the LCD program. The user's input will show
 * up on the LCD screen. The user can type \n to go to a new line
 * on the LCD screen and \c to clear the display.
 */

// Includes the necessary libraries for the functions used
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_INPUT 100	// Maximum input length from the user

int main() {
  char * path = "/root/pipe";	// Pathname for the pipe
  FILE * shared;
  shared = fopen(path, "w");	// Opens the pipe from the LCD program
  if (shared == NULL) {
    printf("Error: fopen sender side failed\n");
    return 1;
  }

  printf("Enter input to send to the LCD display.\n");
  printf("Enter \\n to go to the next line, ");
  printf("or enter \\c to clear the display.\n\n");
  
  char temp[MAX_INPUT];  // Stores the input from the user
  char * input = temp;
  printf("Enter input: ");
  
  // Takes the user's input and sends it to the LCD until the user 
  // inputs an EOF character (Ctrl-D)
  while (fgets(temp, MAX_INPUT, stdin) != NULL) {
    fprintf(shared, "%s", input);
    fflush(shared);
    printf("Enter input: ");
  }
  printf("\n");

  fclose(shared);  // Closes the connection to the pipe
  return 0;
}
