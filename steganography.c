#include "steganography.h"
#include "utils.h"

// Cross-reference pixel values between original and stego files
int crossReferencePixels(FILE* originalFile, FILE* stegoFile, long imageSize) {
    // Allocate memory to hold pixel data from original and stego files
    uint8_t* originalPixels = (uint8_t*)malloc(imageSize);
    uint8_t* stegoPixels = (uint8_t*)malloc(imageSize);

    // Check if memory allocation was successful
    if (!originalPixels || !stegoPixels) {
        // If not, print an error message and free any allocated memory
        fprintf(stderr, "Memory allocation failed.\n");
        free(originalPixels);
        free(stegoPixels);
        return GENERAL_ERROR;
    }

    // Read pixel data from original and stego files into allocated memory
    fread(originalPixels, 1, imageSize, originalFile);
    fread(stegoPixels, 1, imageSize, stegoFile);

    // Loop through each pixel and compare the values
    for (long i = 0; i < imageSize; i++) {
        // If a mismatch is found, print the details
        if (originalPixels[i] != stegoPixels[i]) {
            fprintf(stderr, "Pixel mismatch at index %ld: original = %d, stego = %d\n", i, originalPixels[i], stegoPixels[i]);
        }
    }

    // Free the allocated memory
    free(originalPixels);
    free(stegoPixels);

    // Return success
    return SUCCESSFUL;
}

// Hide data within a BMP file
int hideData(FILE* inputFile, FILE* coverFile, FILE* outputFile, int bits_to_hide) {
    // Move the file pointer to the end of the input file to get its size
    fseek(inputFile, 0, SEEK_END);
    long inputFileSize = ftell(inputFile);
    rewind(inputFile); // Move the file pointer back to the beginning

    // Calculate the total input size including the terminator sequence
    size_t terminatorLength = strlen(TERMINATOR_SEQUENCE);
    long totalInputSize = inputFileSize + terminatorLength;

    // Allocate memory to hold the input data and terminator sequence
    uint8_t* inputData = (uint8_t*)malloc(totalInputSize);
    if (!inputData) {
        // If memory allocation fails, print an error message
        fprintf(stderr, "Memory allocation failed.\n");
        return GENERAL_ERROR;
    }

    // Read the input data into memory
    fread(inputData, 1, inputFileSize, inputFile);
    // Append the terminator sequence to the input data
    memcpy(inputData + inputFileSize, TERMINATOR_SEQUENCE, terminatorLength);

    // Read the BMP header from the cover file
    uint8_t header[54];
    fread(header, 1, 54, coverFile);
    // Write the BMP header to the output file
    fwrite(header, 1, 54, outputFile);

    // Read the first pixel from the cover file
    uint8_t bits_pixel[3];
    fread(bits_pixel, 1, 3, coverFile);
    // Store the number of bits to hide in the first pixel
    bits_pixel[0] = embedBits(bits_pixel[0], bits_to_hide, 4); // Store in the least significant 4 bits
    // Write the modified first pixel to the output file
    fwrite(bits_pixel, 1, 3, outputFile);

    // Calculate the total number of bits to hide
    int totalBitsToHide = totalInputSize * 8;
    int bitsHidden = 0;

    // Loop until all bits are hidden
    while (bitsHidden < totalBitsToHide) {
        // Read 4 pixels (12 bytes) from the cover file
        uint8_t pixels[4 * 3]; // Always 4 pixels
        size_t readCount = fread(pixels, 1, 4 * 3, coverFile);
        if (readCount < 4 * 3) {
            // Handle insufficient pixels
            if (readCount >= 3) {
                // Calculate the average color of the pixels
                uint8_t avg[3];
                averageColors(avg, pixels);

                // Prepare to hide bits in the average color
                uint8_t bits[3];
                for (int i = 0; i < 3 && bitsHidden < totalBitsToHide; ++i) {
                    int byteIndex = bitsHidden / 8;
                    int bitOffset = bitsHidden % 8;
                    // Extract the bits to hide from the input data
                    bits[i] = (inputData[byteIndex] >> (8 - bits_to_hide - bitOffset)) & ((1 << bits_to_hide) - 1);
                    bitsHidden += bits_to_hide;
                }

                // Distribute the modified average color back to the pixels
                distributeAverage(avg, pixels, bits_to_hide, bits);
                // Write the modified pixels to the output file
                fwrite(pixels, 1, readCount, outputFile);
            }
            break;
        }

        // Calculate the average color of the pixels
        uint8_t avg[3];
        averageColors(avg, pixels);

        // Prepare to hide bits in the average color
        uint8_t bits[3];
        for (int i = 0; i < 3 && bitsHidden < totalBitsToHide; ++i) {
            int byteIndex = bitsHidden / 8;
            int bitOffset = bitsHidden % 8;
            // Extract the bits to hide from the input data
            bits[i] = (inputData[byteIndex] >> (8 - bits_to_hide - bitOffset)) & ((1 << bits_to_hide) - 1);
            bitsHidden += bits_to_hide;
        }

        // Distribute the modified average color back to the pixels
        distributeAverage(avg, pixels, bits_to_hide, bits);
        // Write the modified pixels to the output file
        fwrite(pixels, 1, 4 * 3, outputFile);
    }

    // Write any remaining data from the cover file to the output file
    uint8_t buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), coverFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    // Free the allocated memory for input data
    free(inputData);
    return SUCCESSFUL;
}

// Extract hidden data from a BMP file
int extractData(FILE* stegoFile, FILE* outputFile, int bits_to_hide) {
    // Read the BMP header from the stego file
    uint8_t header[54];
    fread(header, 1, 54, stegoFile);

    // Read the first pixel from the stego file
    uint8_t bits_pixel[3];
    fread(bits_pixel, 1, 3, stegoFile);
    // Extract the number of bits used for hiding from the first pixel
    int hidden_bits_to_hide = extractBits(bits_pixel[0], 4);

    // Check if the provided bits_to_hide matches the hidden_bits_to_hide
    if (bits_to_hide != hidden_bits_to_hide) {
        fprintf(stderr, "Error: Number of bits for extraction does not match the number of bits used for hiding.\n");
        return PARAMETERS_PROVIDED_INCORRECT_ERROR;
    }

    // Initialize variables for extracting data
    const int BITS_IN_BYTE = 8;
    int bitsExtracted = 0;
    size_t extractedSize = 0;
    size_t allocatedSize = 1024;
    // Allocate memory to hold the extracted data
    uint8_t* extractedData = (uint8_t*)malloc(allocatedSize);
    if (!extractedData) {
        fprintf(stderr, "Memory allocation failed.\n");
        return GENERAL_ERROR;
    }

    // Loop until all bits are extracted
    while (1) {
        // Read 4 pixels (12 bytes) from the stego file
        uint8_t pixels[4 * 3]; // Always 4 pixels
        size_t readCount = fread(pixels, 1, 4 * 3, stegoFile);
        if (readCount < 4 * 3) {
            // Handle insufficient pixels
            if (readCount >= 3) {
                for (int i = 0; i < 3; ++i) {
                    if (bitsExtracted % BITS_IN_BYTE == 0) {
                        if (extractedSize >= allocatedSize) {
                            // Reallocate memory if needed
                            allocatedSize *= 2;
                            extractedData = (uint8_t*)realloc(extractedData, allocatedSize);
                            if (!extractedData) {
                                fprintf(stderr, "Memory reallocation failed.\n");
                                free(extractedData);
                                return GENERAL_ERROR;
                            }
                        }
                        // Initialize a new byte for extracted data
                        extractedData[extractedSize] = 0;
                        extractedSize++;
                    }

                    // Extract the bits from the pixel and store them in the extracted data
                    int shift = BITS_IN_BYTE - bitsExtracted % BITS_IN_BYTE - bits_to_hide;
                    uint8_t bits = extractBits(pixels[i], bits_to_hide) << shift;
                    extractedData[extractedSize - 1] |= bits;
                    bitsExtracted += bits_to_hide;
                }
            }
            break;
        }

        // Calculate the average color of the pixels
        uint8_t avg[3];
        averageColors(avg, pixels);

        for (int i = 0; i < 3; ++i) {
            if (bitsExtracted % BITS_IN_BYTE == 0) {
                if (extractedSize >= allocatedSize) {
                    // Reallocate memory if needed
                    allocatedSize *= 2;
                    extractedData = (uint8_t*)realloc(extractedData, allocatedSize);
                    if (!extractedData) {
                        fprintf(stderr, "Memory reallocation failed.\n");
                        free(extractedData);
                        return GENERAL_ERROR;
                    }
                }
                // Initialize a new byte for extracted data
                extractedData[extractedSize] = 0;
                extractedSize++;
            }

            // Extract the bits from the average color and store them in the extracted data
            int shift = BITS_IN_BYTE - bitsExtracted % BITS_IN_BYTE - bits_to_hide;
            uint8_t bits = extractBits(avg[i], bits_to_hide) << shift;
            extractedData[extractedSize - 1] |= bits;
            bitsExtracted += bits_to_hide;
        }

        // Check if the terminator sequence is reached
        if (extractedSize >= strlen(TERMINATOR_SEQUENCE)) {
            if (memcmp(extractedData + extractedSize - strlen(TERMINATOR_SEQUENCE), TERMINATOR_SEQUENCE, strlen(TERMINATOR_SEQUENCE)) == 0) {
                // Remove the terminator sequence from the extracted data
                extractedSize -= strlen(TERMINATOR_SEQUENCE);
                break;
            }
        }
    }

    // Write the extracted data to the output file
    fwrite(extractedData, 1, extractedSize, outputFile);
    // Free the allocated memory for extracted data
    free(extractedData);
    return SUCCESSFUL;
}

// Embed the given bits into the color component
uint8_t embedBits(uint8_t color, uint8_t bits, uint8_t num_bits) {
    // Clear the least significant bits in the color component
    color &= ~((1 << num_bits) - 1);
    // Set the new bits in the least significant bits
    color |= bits;
    return color;
}

// Extract the least significant bits from the color component
uint8_t extractBits(uint8_t color, uint8_t num_bits) {
    // Extract the least significant bits
    return color & ((1 << num_bits) - 1);
}

// Calculate the average color components from the given pixels
void averageColors(uint8_t* avg, uint8_t* pixels) {
    // Initialize variables for summing color components
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < 4 * 3; i += 3) {
        // Sum the color components of each pixel
        r += pixels[i];
        g += pixels[i + 1];
        b += pixels[i + 2];
    }
    // Calculate the average color components
    avg[0] = r / 4;
    avg[1] = g / 4;
    avg[2] = b / 4;
}

// Distribute the average color with embedded bits to the pixels
void distributeAverage(uint8_t* avg, uint8_t* pixels, int bits_to_hide, uint8_t* bits) {
    // Embed the desired bits into the average color
    for (int i = 0; i < 3; ++i) {
        avg[i] = embedBits(avg[i], bits[i], bits_to_hide);
    }

    // Calculate the target sum for each color component
    int targetR = avg[0] * 4;
    int targetG = avg[1] * 4;
    int targetB = avg[2] * 4;

    // Initialize variables for the current sum of color components
    int currentR = 0, currentG = 0, currentB = 0;

    // Calculate the current sum of color components
    for (int i = 0; i < 4 * 3; i += 3) {
        currentR += pixels[i];
        currentG += pixels[i + 1];
        currentB += pixels[i + 2];
    }

    // Calculate the difference needed to adjust the average
    int diffR = targetR - currentR;
    int diffG = targetG - currentG;
    int diffB = targetB - currentB;

    // Adjust the pixels to achieve the desired average
    adjustPixels(pixels, 0, diffR);
    adjustPixels(pixels, 1, diffG);
    adjustPixels(pixels, 2, diffB);
}

// Adjust pixel values to achieve the desired average with embedded bits
void adjustPixels(uint8_t* pixels, int component_index, int diff) {
    int totalAdjustment = 0;
    for (int i = 0; i < 4 * 3; i += 3) {
        // Calculate the adjustment needed for each pixel
        int adjustment = diff / 4;
        if (i / 3 < diff % 4) {
            adjustment += (diff > 0) ? 1 : -1;
        }
        // Adjust the pixel value
        pixels[i + component_index] += adjustment;
        totalAdjustment += adjustment;
    }

    // Calculate the final adjustment needed
    int finalAdjustment = diff - totalAdjustment;
    if (finalAdjustment != 0) {
        // Apply the final adjustment to the remaining pixels
        for (int i = 0; i < 4 * 3; i += 3) {
            if (finalAdjustment == 0) break;
            pixels[i + component_index] += (finalAdjustment > 0) ? 1 : -1;
            finalAdjustment += (finalAdjustment > 0) ? -1 : 1;
        }
    }
}
