// unzip.h
#ifndef UNZIP_H
#define UNZIP_H
#include <stdio.h>
# include "Utils.h"
#include "zipFile.h"  // Ensure this is included for Node type and other declarations

void unzipFile(); // No parameters now
void lz77Decompress(FILE *compressedFile, FILE *outputFile);
void huffmanDecode(FILE *huffmanEncodedFile, FILE *outputFile, Node *root);
Node *buildHuffmanTreeFromCodes(FILE *huffmanCodesFile);

#endif // UNZIP_H

