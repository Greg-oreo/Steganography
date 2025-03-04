#include "steganography.h"
#include "utils.h"

// Global variable to store bits used for hiding is declared in utils.h

int main(int argc, char *argv[]) {
    // If no arguments are provided, display the usage menu
    if (argc == 1) {
        displayMenu();
        return 0;
    }

    // Initialize variables
    int selection = 0;
    FILE* inputFile = NULL;
    FILE* coverFile = NULL;
    FILE* stegoFile = NULL;
    FILE* outputFile = NULL;
    const char* mf = NULL;
    const char* cf = NULL;
    const char* sf = NULL;
    const char* of = NULL;
    int optional = 0;
    int bits_to_hide = 2;

    // Check command line parameters and validate them
    int result = checkParams(argc, argv, &selection, &optional, &bits_to_hide);
    if (result) {
        // If parameters are incorrect, print an error message and return the error code
        fprintf(stderr, "Closing program. [Error %d]\n", result);
        return result;
    }

    // Set the global variable for bits to hide
    global_bits_to_hide = bits_to_hide;

    // Process based on selection (hide or extract)
    if (!selection) { // If selection is hide
        mf = argv[3]; // Message file
        cf = argv[5]; // Cover file
        if (!optional) {
            of = DEFAULT_HIDE_OUTPUT_FILE; // Default output file if optional output file is not provided
        } else {
            if (argc < 10 || strlen(argv[9]) == 0) {
                fprintf(stderr, "Error: Output file not specified.\n");
                return INCORRECT_NUM_PARAMETERS;
            }
            of = argv[9]; // Optional output file
        }
        // Check access and open input files for reading
        result = fileAccessCheck((char*)mf, &inputFile, READ_FILE);
        if (result) return result;
        result = fileAccessCheck((char*)cf, &coverFile, READ_FILE);
        if (result) return result;
        // Check access and open output file for writing
        result = fileAccessCheck((char*)of, &outputFile, WRITE_FILE);
        if (result) return result;
        // Hide data in the BMP file
        result = hideData(inputFile, coverFile, outputFile, bits_to_hide);
        if (result) {
            // If there is an error in hiding data, print an error message and return the error code
            fprintf(stderr, "Error hiding data. [Error %d]\n", result);
            return result;
        } else {
            // If data is successfully hidden, print a success message
            printf("Data successfully hidden in %s.\n", of);
        }
    } else { // If selection is extract
        sf = argv[3]; // Stego file
        if (!optional) {
            of = DEFAULT_EXTRACT_OUTPUT_FILE; // Default output file if optional output file is not provided
        } else {
            if (argc < 8 || strlen(argv[7]) == 0) {
                fprintf(stderr, "Error: Output file not specified.\n");
                return INCORRECT_NUM_PARAMETERS;
            }
            of = argv[7]; // Optional output file
        }
        // Check access and open stego file for reading
        result = fileAccessCheck((char*)sf, &stegoFile, READ_FILE);
        if (result) return result;
        // Open output file for writing
        outputFile = fopen(of, "wb");
        if (!outputFile) {
            // If there is an error in opening the output file, print an error message and return the error code
            fprintf(stderr, "Error: Unable to create or write to the file: %s\n", of);
            return FILE_ACCESS_ERROR;
        }
        // Extract data from the BMP file
        result = extractData(stegoFile, outputFile, bits_to_hide);
        if (result) {
            // If there is an error in extracting data, print an error message, close and remove the output file, and return the error code
            fprintf(stderr, "Error extracting data. [Error %d]\n", result);
            fclose(outputFile);
            remove(of);  // Remove the output file
            return result;
        } else {
            // If data is successfully extracted, print a success message
            printf("Data successfully extracted to %s.\n", of);
        }
    }

    // Close all file pointers
    if (inputFile) fclose(inputFile);
    if (coverFile) fclose(coverFile);
    if (stegoFile) fclose(stegoFile);
    if (outputFile) fclose(outputFile);

    return 0; // Return success code
}
