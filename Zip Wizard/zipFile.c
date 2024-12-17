#include "Utils.h"
#include "zipFile.h"

void zipfile() {
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char compressed_file[MAX_FILE_NAME_LENGTH + 1];

    zwPrint("Enter the name of the file to be zipped:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline character

    snprintf(compressed_file, sizeof(compressed_file), "%s.zip", file_name);
    FILE *fileName = fopen(file_name, "rb");
    FILE *zipFile = fopen(compressed_file, "wb");

    if (!fileName || !zipFile) {
        zwPrint("Error: Unable to open files for zipping.\n", 20, ERROR_FILE);
        if (fileName) fclose(fileName);
        return;
    }

    // Step 5: Perform LZ77 compression
    lz77(fileName, zipFile);
    fclose(fileName);
    fclose(zipFile);

    zwPrint("File zipped successfully.\n", 20, SUCCESS);
}

void lz77(FILE *inputFile, FILE *compressedFile) {
    char window[WINDOW_SIZE] = {0};
    char buffer[BUFFER_SIZE + 1] = {0};
    int offset, length;
    char nextChar;

    int windowStart = 0;
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, inputFile)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        char *current = buffer;

        while (*current) {
            int maxMatchLen = 0;
            offset = 0;

            // Search for matches in the sliding window
            for (int i = 0; i < windowStart && *current; i++) {
                int j = 0;
                while (window[i + j] == *current && j < BUFFER_SIZE && window[i + j] != '\0') {
                    j++;
                }
                if (j > maxMatchLen) {
                    maxMatchLen = j;
                    offset = windowStart - i;
                }
            }

            // Determine the next character to output
            nextChar = *(current + maxMatchLen);
            length = maxMatchLen;

            // Write (offset, length, nextChar) to the compressed file
            fwrite(&offset, sizeof(int), 1, compressedFile);
            fwrite(&length, sizeof(int), 1, compressedFile);
            fwrite(&nextChar, sizeof(char), 1, compressedFile);

            // Update the sliding window
            if (windowStart + length + 1 <= WINDOW_SIZE) {
                strncpy(window + windowStart, current, length + 1);
                windowStart += length + 1;
            } else {
                int shift = length + 1;
                memmove(window, window + shift, windowStart - shift);
                windowStart -= shift;
                strncpy(window + windowStart, current, length + 1);
                windowStart += length + 1;
            }

            current += length + 1; // Move to the next character
        }
    }
}
