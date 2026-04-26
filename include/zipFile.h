// zipFile.h
#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <stdio.h>
#include "Utils.h"

void zipfile(void);
int zipFilePath(const char *inputFileName, const char *outputZipName);
int batchZipFiles(int count, const char *files[]);
int benchmarkCompressionAlgorithms(const char *inputFileName);

#endif // ZIPFILE_H
