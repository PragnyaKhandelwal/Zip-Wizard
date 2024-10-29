#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <windows.h>
// Color codes
#define INFO 15                  // white
#define PROCESSING_STATEMENTS 11 // cyan
#define WARNING 6                // orange (closest is yellow)
#define ERROR_FILE 12            // red
#define SUCCESS 10
#define MAX_FILE_NAME_LENGTH 99  // maximum length of file name
#define MAX_CONTENT_LENGTH 999
#define MAX_OUTPUT_LINES 50
#define MAX_TREE_NODES 256

void zwPrint(const char *text, int offset, int type);

#endif
