// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// fileinput.c
// CSE/EE 474 Lab 2
// 4/21/2017

/* This program takes in files as arguments and sends the contents
 * of the files to the music playing/printing program. It can 
 * accept multiple file arguments.
 */

// Includes the necessary libraries for the functions used
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_INPUT 100	// Maximum input length from the user

int main (int argc, char *argv[]) {
  char * path = "/root/pipe";	// Pathname for the pipe
  FILE * shared;
  int num = 1;
  shared = fopen(path, "w");	// Opens the pipe from the LCD program
  if (shared == NULL) {
    printf("fopen sender side failed\n");
    return 1;
  }

  char temp[MAX_INPUT];	 // Stores the input from the user
  char * input = temp;
  FILE * fp;	// Current file that the program is reading

  // Takes the input files and reads each line to the output
  while (num < argc) {
    fp = fopen(argv[num], "r");		// Opens the input file
    if (!fp) {
      printf("Error: cannot open file\n");
      return 1;      
      printf("%s", "reading from file\n");
    }
	// Reads each line of the input file
    while (fgets(temp, MAX_INPUT, fp) != NULL) {
      fprintf(shared, "%s", input);
      fflush(shared);
    }
    num++;
  }
  printf("\n");
  
  fclose(shared);	// Closes the connection to the pipe
  return 0;
}
