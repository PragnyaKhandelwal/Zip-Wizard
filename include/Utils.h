#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h>
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
#define WINDOW_SIZE 1024
#define BUFFER_SIZE 256

void terminalSize(int width, int height);
void zwPrint(const char *text, int offset, int type);
void zwPrintInline(const char *text, int offset, int type);
void zwReadLine(char *buffer, size_t size, int offset);
void zwClearScreen(void);
void zwDrawRule(int offset, int width, char ch, int type);
void zwMoveCursor(int offset);

#endif
