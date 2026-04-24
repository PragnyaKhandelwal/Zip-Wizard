#include "zipFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzipFile.h"

void lz77Decompress(FILE *compressedFile, FILE *outputFile) {
    int offset, length;
    unsigned char nextChar; // Match unsigned char type from compression
    unsigned char window[WINDOW_SIZE] = {0}; // Circular sliding window
    int windowPos = 0; // Current write/absolute position

    // Read until the end of the compressed file
    while (fread(&offset, sizeof(int), 1, compressedFile) == 1 &&
           fread(&length, sizeof(int), 1, compressedFile) == 1 &&
           fread(&nextChar, sizeof(unsigned char), 1, compressedFile) == 1) {

        // Decompress the data from the sliding window
        if (length > 0) {
            // Calculate starting read position relative to the absolute window position
            int readPos = windowPos - offset;
            
            // Wrap around if the position points before the start of the buffer
            while (readPos < 0) {
                readPos += WINDOW_SIZE;
            }

            // Write the matching sequence
            for (int i = 0; i < length; i++) {
                unsigned char c = window[readPos % WINDOW_SIZE];
                fputc(c, outputFile);
                
                // Add to the current write position in the bounds of the window
                window[windowPos % WINDOW_SIZE] = c;
                windowPos++;
                readPos++;
            }
        }

        // Write the next literal character to the output
        fputc(nextChar, outputFile);
        window[windowPos % WINDOW_SIZE] = nextChar;
        windowPos++;
    }
}
void unzipFile() {
    char compressedFileName[MAX_FILE_NAME_LENGTH + 1];
    char outputFileName[MAX_FILE_NAME_LENGTH + 1];

    // Get the name of the compressed file from the user
    zwPrintInline("Compressed file (.zip): ", 20, INFO);
    zwReadLine(compressedFileName, sizeof(compressedFileName), 44);

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
}
