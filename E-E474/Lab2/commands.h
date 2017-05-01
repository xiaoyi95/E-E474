// Pezhman Khorasani, Xiaoyi Ling, Tiffany Luu
// commands.h
// CSE/EE 474 Lab 2
// 4/21/2017

// Header file for GPIO access and LCD commands

#ifndef COMMANDS_H
#define COMMANDS_H

void gpioout(FILE *pin, int val);
void gpiodir(FILE *pin, char *dir);
void gpioexport(FILE *export, int pnum);
FILE * createdir(int pnum);
FILE * createval(int pnum);
void enable(FILE *pin);
void instr(FILE *sel, FILE *rw, int choose);
void writechar(FILE *sel, FILE *rw, FILE *en, FILE *val[8], int *out);
void leftshift(FILE *sel, FILE *rw, FILE *en, FILE *val[8]);
void rtnhome(FILE *sel, FILE *rw, FILE *en, FILE *val[8]);
void sndline(FILE *sel, FILE *rw, FILE *en, FILE *val[8]);
void clrdisplay(FILE *sel, FILE *rw, FILE *en, FILE *val[8]);
void initialize(FILE *sel, FILE *rw, FILE *en, FILE *val[8]);

#endif
