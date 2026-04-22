// menu.c
#include "menu.h"
#include "createFile.h"
#include "editFile.h"
#include "renameFile.h"
#include "deleteFile.h"
#include "searchFile.h"
#include "fileInfo.h"
#include "zipFile.h"
#include "unzipFile.h"
#include "fileIndex.h"
#include "Utils.h"
// Required system headers
#include <stdio.h>
#include <stdlib.h>  // For exit()
#include <string.h>  // For strlen, strcspn
#include <conio.h>   // For _kbhit()

// Windows-specific headers for console manipulation
#include <windows.h> // For GetStdHandle, SetConsoleScreenBufferSize

// Define the global variables
int outputCount = 0;  // Output counter
char outputLog[MAX_OUTPUT_LINES][100];  // Output log array

static void runTimedOperation(const char *operationName, void (*operation)(void)) {
    LARGE_INTEGER start, end, frequency;
    char timingMessage[160];

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    operation();
    QueryPerformanceCounter(&end);

    double elapsedMs = ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)frequency.QuadPart;
    snprintf(timingMessage, sizeof(timingMessage), "[Timing] %s completed in %.3f ms", operationName, elapsedMs);
    zwPrint(timingMessage, 20, INFO);
}

// Define the menu function
void menu() {
    zwClearScreen();
    printf("\n");
    zwDrawBoxSimple(80, PROCESSING_STATEMENTS);
    zwPrintCentered("╔═══════════════════════════════════════════════════════════╗", INFO);
    zwPrintCentered("║          ✨ ZIP WIZARD - PROFESSIONAL FILE UTILITY ✨       ║", SUCCESS);
    zwPrintCentered("╚═══════════════════════════════════════════════════════════╝", INFO);
    zwDrawBoxSimple(80, PROCESSING_STATEMENTS);

    printf("\n");
    zwPrintCentered("📋 CHOOSE AN OPERATION:", INFO);
    printf("\n");

    zwMenuItemStyled(1, "📄 Create File", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(2, "✏️  Edit File", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(3, "🔄 Rename File", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(4, "🗑️  Delete File", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(5, "🔍 File Search (KMP)", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(6, "📊 File Information", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(7, "📦 Zip File (LZ77)", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(8, "📂 Unzip File", 15, PROCESSING_STATEMENTS);
    zwMenuItemStyled(9, "🚪 Exit Program", 15, WARNING);

    printf("\n");
    zwDrawBoxSimple(80, PROCESSING_STATEMENTS);
    printf("\n");
}

// Define the heading function
void heading(const char *text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD bufferSize = {100, 100}; // Buffer size must be at least as large as the window size
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1; // Width of the console
    // Calculate the position to center the text
    int textLength = (int)strlen(text);
    int pos = (consoleWidth - textLength) / 2;
    zwPrint(text, pos, INFO);
}

// Define the userchoice function
void userchoice(int n) {
    printf("\n");
    switch (n) {
    case 1:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("📄 Creating a file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Create File", createfile);
        break;
    case 2:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("✏️  Editing the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Edit File", editfile);
        break;
    case 3:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("🔄 Renaming the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Rename File", renamefile);
        break;
    case 4:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("🗑️  Deleting the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Delete File", deletefile);
        break;
    case 5:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("🔍 Searching the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Search File", searchfile);
        break;
    case 6:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("📊 Fetching file information...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("File Information", fileinfo);
        break;
    case 7:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("📦 Zipping the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Zip File", zipfile);
        break;
    case 8:
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        zwPrintCentered("📂 Unzipping the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("──────────────────────────────────────────────────────────", INFO);
        runTimedOperation("Unzip File", unzipFile);
        break;
    }
    printf("\n  ► Press Enter to return to the main menu...");
    char dummy[10];
    fgets(dummy, sizeof(dummy), stdin);
}

// Define the quitProgram function
void quitProgram() {
    printf("\n");
    zwPrintCentered("═══════════════════════════════════════════════════════════", INFO);
    zwPrintCentered("Exiting the program...", PROCESSING_STATEMENTS);
    zwPrintCentered("Thank you for using ZIP WIZARD! 👋", SUCCESS);
    zwPrintCentered("═══════════════════════════════════════════════════════════", INFO);
    printf("\n");
    fileIndexShutdown();
    exit(0);
}

// Define the validatechoices function
// Define the validatechoices function
void validatechoices() {
    int choice = -1;

    // Prompt for input - NEW ALIGNED APPROACH
    printf("\n  ➤ Enter your choice (1-9): ");
    fflush(stdout);

    // Continuously prompt the user until a valid input is entered
    while (1) {
        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            zwPrint("Error: Failed to read input.\n", 2, ERROR_FILE);
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        // Check for empty input
        if (strlen(input) == 0) {
            zwPrint("  ✗ Error: Input cannot be empty. Please enter a number from 1 to 9.\n", 2, WARNING);
            printf("  ➤ Enter your choice (1-9): ");
            fflush(stdout);
            continue;
        }

        // Try to convert input to an integer
        if (sscanf(input, "%d", &choice) != 1) {
            zwPrint("  ✗ Error: The input must be an integer.\n", 2, ERROR_FILE);
            printf("  ➤ Enter your choice (1-9): ");
            fflush(stdout);
            continue;
        }

        // Validate the choice
        if (choice >= 1 && choice <= 8) {
            userchoice(choice);  // Proceed with the selected action
            break;  // Exit the loop once a valid choice is processed
        }
        else if (choice == 9) {
            quitProgram();  // Exit the program
            break;  // Exit the loop
        }
        else {
            zwPrint("  ✗ Error: Invalid Input! Please enter a number between 1 and 9.\n", 2, WARNING);
            printf("  ➤ Enter your choice (1-9): ");
            fflush(stdout);
        }
    }
}
