// unzipFile.h
#ifndef UNZIPFILE_H
#define UNZIPFILE_H

#include <stdio.h>
#include "Utils.h"

void unzipFile(void);
void lz77Decompress(FILE *compressedFile, FILE *outputFile);
int unzipFilePath(const char *compressedFileName, const char *outputFileName);
int batchUnzipFiles(int count, const char *files[]);

#endif // UNZIPFILE_H
