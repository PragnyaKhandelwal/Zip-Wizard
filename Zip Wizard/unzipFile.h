// unzip.h
#ifndef UNZIP_H
#define UNZIP_H

#include <stdio.h>
#include "Utils.h"
#include "zipFile.h"  // Ensure this is included for Node type and other declarations

void unzipFile(); // Function to unzip a file
void lz77Decompress(FILE *compressedFile, FILE *outputFile); // LZ77 decompression

#endif // UNZIP_H
