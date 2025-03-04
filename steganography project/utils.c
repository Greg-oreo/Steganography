#include "utils.h"
#include <string.h>
#include <unistd.h>

int global_bits_to_hide = -1; // Initialize the global variable to store bits used for hiding

// Check command line parameters
int checkParams(const int arguments, char* const list[], int* selection, int* optional, int* bits_to_hide) {
    // Check if the number of arguments is correct (should be either 8 or 10 for hide, 6 or 8 for extract)
    if ((strncmp(list[1], HIDE, strlen(HIDE)) == 0 && arguments != 8 && arguments != 10) ||
        (strncmp(list[1], EXTRACT, strlen(EXTRACT)) == 0 && arguments != 6 && arguments != 8)) {
        fprintf(stderr, "Incorrect number of parameters. Provided: %d\n", arguments);
        return INCORRECT_NUM_PARAMETERS;
    }

    // Check if the first argument is the hide command
    if (strncmp(list[1], HIDE, strlen(HIDE)) == 0) {
        *selection = 0; // Set selection to hide
        
        // Check if the message flag is correct
        if (strncmp(list[2], MSG_FLAG, strlen(MSG_FLAG)) != 0) {
            fprintf(stderr, "Missing or incorrect message flag.\n");
            return MSG_ERROR;
        }

        // Check if the cover flag is correct
        if (strncmp(list[4], COVER_FLAG, strlen(COVER_FLAG)) != 0) {
            fprintf(stderr, "Missing or incorrect cover flag.\n");
            return COVER_ERROR;
        }

        // Check if the bits flag is correct
        if (strncmp(list[6], BITS, strlen(BITS)) != 0) {
            fprintf(stderr, "Missing or incorrect bits flag.\n");
            return PARAMETERS_PROVIDED_INCORRECT_ERROR;
        }

        // Convert the bits argument to an integer and store it
        *bits_to_hide = atoi(list[7]);

        // If optional arguments are provided, check if the optional flag is correct
        if (arguments == 10) {
            if (strncmp(list[8], OPTIONAL_FLAG, strlen(OPTIONAL_FLAG)) != 0) {
                fprintf(stderr, "Missing or incorrect optional flag.\n");
                return OPTIONAL_ERROR;
            }
            if (list[9] == NULL || strlen(list[9]) == 0) {
                fprintf(stderr, "Missing output file name.\n");
                return INCORRECT_NUM_PARAMETERS;
            }
            *optional = 1; // Set optional flag to true
        }

    // Check if the first argument is the extract command
    } else if (strncmp(list[1], EXTRACT, strlen(EXTRACT)) == 0) {
        *selection = 1; // Set selection to extract
        
        // Check if the stego flag is correct
        if (strncmp(list[2], STEGO_FLAG, strlen(STEGO_FLAG)) != 0) {
            fprintf(stderr, "Missing or incorrect stego flag.\n");
            return STEGO_ERROR;
        }

        // Check if the bits flag is correct
        if (strncmp(list[4], BITS, strlen(BITS)) != 0) {
            fprintf(stderr, "Missing or incorrect bits flag.\n");
            return PARAMETERS_PROVIDED_INCORRECT_ERROR;
        }

        // Convert the bits argument to an integer and store it
        *bits_to_hide = atoi(list[5]);

        // If optional arguments are provided, check if the optional flag is correct
        if (arguments == 8) {
            if (strncmp(list[6], OPTIONAL_FLAG, strlen(OPTIONAL_FLAG)) != 0) {
                fprintf(stderr, "Missing or incorrect optional flag.\n");
                return OPTIONAL_ERROR;
            }
            if (list[7] == NULL || strlen(list[7]) == 0) {
                fprintf(stderr, "Missing output file name.\n");
                return INCORRECT_NUM_PARAMETERS;
            }
            *optional = 1; // Set optional flag to true
        }

    // If the first argument is not hide or extract, print an error message and return error code for incorrect first parameter
    } else {
        fprintf(stderr, "First parameter is incorrect. Provided: %s\n", list[1]);
        return PARAMETERS_PROVIDED_INCORRECT_ERROR;
    }

    return SUCCESSFUL; // Return success code if all checks pass
}

// Check file access permissions
int fileAccessCheck(char* filename, FILE** fp, int readOrWrite) {
    if (readOrWrite == READ_FILE) { // If the file is to be read
        if (access(filename, F_OK) != 0) { // Check if the file exists
            fprintf(stderr, "Error: File does not exist: %s\n", filename); // Print error message
            return FILE_ACCESS_ERROR; // Return file access error code
        }
        if (access(filename, R_OK) != 0) { // Check if the file can be read
            fprintf(stderr, "Error: Access to read the file denied: %s\n", filename); // Print error message
            return ACCESS_DENIED; // Return access denied error code
        }
        *fp = fopen(filename, "rb"); // Open the file for reading in binary mode
    } else if (readOrWrite == WRITE_FILE) { // If the file is to be written
        *fp = fopen(filename, "wb"); // Open the file for writing in binary mode
        if (!*fp) { // Check if the file cannot be opened
            fprintf(stderr, "Error: Unable to open or create the file: %s\n", filename); // Print error message
            return FILE_ACCESS_ERROR; // Return file access error code
        }
    }
    if (*fp == NULL) { // If the file pointer is NULL
        fprintf(stderr, "Error: Unable to open the file: %s\n", filename); // Print error message
        return FILE_ACCESS_ERROR; // Return file access error code
    }
    return SUCCESSFUL; // Return success code if the file can be accessed
}

// Display the command line usage menu
void displayMenu() {
    printf("Usage: stego [options]\n"); // Print usage instructions
    printf("Options:\n");
    printf("  -hide -m <message_file> -c <cover_file> -b <bits> [-o <output_file>]\n");
    printf("    Hide a message in a BMP file using 4 pixels.\n");
    printf("    -m <message_file> : File containing the message to hide.\n");
    printf("    -c <cover_file>   : BMP file to use as cover.\n");
    printf("    -b <bits>         : Number of bits to use per color component (1-4).\n");
    printf("    -o <output_file>  : (Optional) Output BMP file name. Default is 'output_stego.bmp'.\n");
    printf("  -extract -s <stego_file> -b <bits> [-o <output_file>]\n");
    printf("    Extract a message from a BMP file using 4 pixels.\n");
    printf("    -s <stego_file>   : BMP file containing the hidden message.\n");
    printf("    -b <bits>         : Number of bits used per color component (1-4).\n");
    printf("    -o <output_file>  : (Optional) Output text file name. Default is 'output_message.txt'.\n");
}
