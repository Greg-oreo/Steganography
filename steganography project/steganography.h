
#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int hideData(FILE* inputFile, FILE* coverFile, FILE* outputFile, int bits_to_hide);
int extractData(FILE* stegoFile, FILE* outputFile, int bits_to_hide);
int crossReferencePixels(FILE* originalFile, FILE* stegoFile, long imageSize);

uint8_t embedBits(uint8_t color, uint8_t bits, uint8_t num_bits);
uint8_t extractBits(uint8_t color, uint8_t num_bits);
void averageColors(uint8_t* avg, uint8_t* pixels);
void distributeAverage(uint8_t* avg, uint8_t* pixels, int bits_to_hide, uint8_t* bits);
void adjustPixels(uint8_t* pixels, int component_index, int diff);

#ifdef __cplusplus
}
#endif

#endif
