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

int unzipFilePath(const char *compressedFileName, const char *outputFileName) {
    FILE *compressedFile;
    FILE *outputFile;
    char derivedOutput[MAX_FILE_NAME_LENGTH + 16];
    char *zipExtension;

    if (!compressedFileName || strlen(compressedFileName) == 0) {
        printf("Error: compressed file path is empty.\n");
        return 1;
    }

    if (!outputFileName || strlen(outputFileName) == 0) {
        zipExtension = strstr(compressedFileName, ".zip");
        if (zipExtension) {
            size_t prefixLength = (size_t)(zipExtension - compressedFileName);
            snprintf(derivedOutput, sizeof(derivedOutput), "%.*s_output.txt", (int)prefixLength, compressedFileName);
        } else {
            snprintf(derivedOutput, sizeof(derivedOutput), "%s_output.txt", compressedFileName);
        }
        outputFileName = derivedOutput;
    }

    compressedFile = fopen(compressedFileName, "rb");
    if (!compressedFile) {
        printf("Error: Unable to open compressed file for reading.\n");
        return 1;
    }

    outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fclose(compressedFile);
        printf("Error: Unable to open output file for writing.\n");
        return 1;
    }

    lz77Decompress(compressedFile, outputFile);

    fclose(compressedFile);
    fclose(outputFile);
    printf("Unzipped: %s -> %s\n", compressedFileName, outputFileName);
    return 0;
}

int batchUnzipFiles(int count, const char *files[]) {
    int success = 0;
    int failed = 0;

    if (count <= 0 || !files) {
        return 1;
    }

    for (int i = 0; i < count; i++) {
        int progress = ((i + 1) * 100) / count;
        zwDrawProgressBar(progress, 2, 40, PROCESSING_STATEMENTS);
        printf("\n");

        if (unzipFilePath(files[i], NULL) == 0) {
            success++;
        } else {
            failed++;
        }
    }

    printf("Batch unzip summary: total=%d success=%d failed=%d\n", count, success, failed);
    return failed == 0 ? 0 : 1;
}

void unzipFile() {
    char compressedFileName[MAX_FILE_NAME_LENGTH + 1];

    zwPromptInput("  [UNZIP] Compressed file (.zip): ", compressedFileName, sizeof(compressedFileName), INFO);
    if (unzipFilePath(compressedFileName, NULL) == 0) {
        zwPrint("File decompressed successfully", 2, SUCCESS);
        zwSetLastOperationStatus(0);
    } else {
        zwSetLastOperationStatus(1);
    }
}
