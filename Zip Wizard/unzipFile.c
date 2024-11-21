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
    zwPrint("Enter the name of the compressed file (with .zip): \n", 20, INFO);
    scanf("%s", compressedFileName);

    // Generate the output file name by removing the .zip extension and appending _output.txt
    char *zipExtension = strstr(compressedFileName, ".zip");
    if (zipExtension) {
        size_t prefixLength = zipExtension - compressedFileName;
        snprintf(outputFileName, sizeof(outputFileName), "%.*s_output.txt", (int)prefixLength, compressedFileName);
    } else {
        // If no ".zip" is found, just append "_output.txt" to the input file name
        snprintf(outputFileName, sizeof(outputFileName), "%s_output.txt", compressedFileName);
    }

    // Open the compressed file
    FILE *compressedFile = fopen(compressedFileName, "rb");
    if (!compressedFile) {
        zwPrint("Error: Unable to open compressed file for reading.\n", 20, ERROR_FILE);
        return;
    }

    // Open the output file for writing
    FILE *outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        zwPrint("Error: Unable to open output file for writing.\n", 20, ERROR_FILE);
        fclose(compressedFile);
        return;
    }

    // Call decompression function
    lz77Decompress(compressedFile, outputFile);

    // Close the files
    fclose(compressedFile);
    fclose(outputFile);

    zwPrint("File decompressed successfully\n", 20, SUCCESS);
    while (getchar() != '\n'); // Clear the input buffer
}
