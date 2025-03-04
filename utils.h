#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define R_OK 4
#define W_OK 2
#define F_OK 0

#define HIDE "-hide"
#define EXTRACT "-extract"
#define MSG_FLAG "-m"
#define OPTIONAL_FLAG "-o"
#define COVER_FLAG "-c"
#define STEGO_FLAG "-s"
#define BITS "-b"

#define READ_FILE 0
#define WRITE_FILE 1

#define SUCCESSFUL 0
#define GENERAL_ERROR 1
#define INCORRECT_NUM_PARAMETERS 2
#define PARAMETERS_PROVIDED_INCORRECT_ERROR 3
#define HIDE_ERROR 5
#define EXTRACT_ERROR 6
#define MSG_ERROR 7
#define OPTIONAL_ERROR 8
#define COVER_ERROR 9
#define STEGO_ERROR 10
#define FILE_ACCESS_ERROR 11
#define ACCESS_DENIED 12

#define DEFAULT_HIDE_OUTPUT_FILE "output_stego.bmp"
#define DEFAULT_EXTRACT_OUTPUT_FILE "output_message.txt"
#define TERMINATOR_SEQUENCE "END_OF_MESSAGE"

extern int global_bits_to_hide;  // Declare the global variable

void displayMenu();
int checkParams(const int arguments, char* const list[], int* selection, int* optional, int* bits_to_hide);
int fileAccessCheck(char* filename, FILE** fp, int readOrWrite);

#endif
