// unzipFile.h
#ifndef UNZIPFILE_H
#define UNZIPFILE_H

#include <stdio.h>
#include "Utils.h"

void unzipFile(void);
void lz77Decompress(FILE *compressedFile, FILE *outputFile);

#endif // UNZIPFILE_H
