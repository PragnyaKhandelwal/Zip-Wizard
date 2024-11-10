#include "zipFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzipFile.h"



void lz77Decompress(FILE *compressedFile, FILE *outputFile) {
    int offset, length;
    char nextChar;
    char window[WINDOW_SIZE] = {0}; // Sliding window
    int windowPosition = 0; // Current position in the sliding window

    // Read until the end of the compressed file
    while (fread(&offset, sizeof(int), 1, compressedFile) == 1 &&
           fread(&length, sizeof(int), 1, compressedFile) == 1 &&
           fread(&nextChar, sizeof(char), 1, compressedFile) == 1) {

        // Decompress the data from the sliding window
        if (length > 0) {
            if (offset <= 0 || offset > windowPosition) {
                fprintf(stderr, "Invalid offset: %d for windowPosition: %d\n", offset, windowPosition);
                return; // Error handling
            }

            // Write the decompressed data from the window
            for (int i = 0; i < length; i++) {
                char c = window[windowPosition - offset + i];
                fputc(c, outputFile);
                window[windowPosition++] = c; // Update the window

                // Manage window size
                if (windowPosition >= WINDOW_SIZE) {
                    memmove(window, window + windowPosition - WINDOW_SIZE, WINDOW_SIZE);
                    windowPosition = WINDOW_SIZE; // Adjust position after moving
                }
            }
        }

        // Write the next character to the output
        fputc(nextChar, outputFile);
        window[windowPosition++] = nextChar; // Update the window

        // Manage window size again after adding nextChar
        if (windowPosition >= WINDOW_SIZE) {
            memmove(window, window + windowPosition - WINDOW_SIZE, WINDOW_SIZE);
            windowPosition = WINDOW_SIZE; // Adjust position after moving
        }
    }
}

void unzipFile() {
    char compressedFileName[MAX_FILE_NAME_LENGTH + 1];
    char outputFileName[MAX_FILE_NAME_LENGTH + 1];

    // Get the name of the compressed file from the user
    printf("Enter the name of the compressed file (with .zip): ");
    scanf("%s", compressedFileName);

    // Get the name for the output file
    printf("Enter the name for the output file: ");
    scanf("%s", outputFileName);

    FILE *compressedFile = fopen(compressedFileName, "rb");
    if (!compressedFile) {
        fprintf(stderr, "Error: Unable to open compressed file for reading.\n");
        return;
    }

    FILE *outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Error: Unable to open output file for writing.\n");
        fclose(compressedFile);
        return;
    }

    lz77Decompress(compressedFile, outputFile); // Call to your decompression function

    fclose(compressedFile);
    fclose(outputFile);
    printf("File unzipped successfully.\n");
}

