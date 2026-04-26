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

// ===== LEGACY FUNCTIONS (For backward compatibility) =====
void terminalSize(int width, int height);
void zwPrint(const char *text, int offset, int type);
void zwPrintInline(const char *text, int offset, int type);
void zwReadLine(char *buffer, size_t size, int offset);
void zwClearScreen(void);
void zwDrawRule(int offset, int width, char ch, int type);
void zwMoveCursor(int offset);

// ===== NEW PROFESSIONAL UI FUNCTIONS =====
// Centered text printing
void zwPrintCentered(const char *text, int type);

// Prompt + Input in one function (properly aligned)
void zwPromptInput(const char *prompt, char *buffer, size_t size, int type);

// Box drawing for visual appeal
void zwDrawBox(int width, int height, int offsetX, int offsetY, int type);
void zwDrawBoxSimple(int width, int type);

// Progress bar display
void zwDrawProgressBar(int percentage, int offsetX, int width, int type);

// Input field with visual underline
void zwInputFieldUnderline(const char *prompt, char *buffer, size_t size, int type);

// Styled menu items
void zwMenuItemStyled(int number, const char *description, int offset, int type);

// Clear line at specific position
void zwClearLine(int offsetY);

// Operation status and validation helpers
void zwSetLastOperationStatus(int status);
int zwGetLastOperationStatus(void);
int zwValidateFileName(const char *fileName, char *errorBuf, size_t errorSize);
int zwHasTxtExtension(const char *fileName);

#endif
