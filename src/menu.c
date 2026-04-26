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

typedef struct OperationTelemetry {
    unsigned long totalOps;
    unsigned long successOps;
    unsigned long failedOps;
    double totalMs;
} OperationTelemetry;

static OperationTelemetry g_telemetry = {0, 0, 0, 0.0};

static void getConsoleDimensions(int *width, int *height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *width = 110;
        *height = 35;
    }
}

static int getOuterRuleWidth(void) {
    int w, h;
    (void)h;
    getConsoleDimensions(&w, &h);
    if (w < 30) {
        return 28;
    }
    return w - 2;
}

static int getMenuOffset(void) {
    int w, h;
    int panelWidth = 61;
    (void)h;
    getConsoleDimensions(&w, &h);
    if (w <= panelWidth + 2) {
        return 2;
    }
    return (w - panelWidth) / 2;
}

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

    g_telemetry.totalOps++;
    g_telemetry.totalMs += elapsedMs;
    if (zwGetLastOperationStatus() == 0) {
        g_telemetry.successOps++;
    } else {
        g_telemetry.failedOps++;
    }
}

// Define the menu function
void menu() {
    zwClearScreen();
    printf("\n");
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);
    zwPrintCentered("+-----------------------------------------------------------+", INFO);
    zwPrintCentered("|          ZIP WIZARD - PROFESSIONAL FILE UTILITY          |", SUCCESS);
    zwPrintCentered("+-----------------------------------------------------------+", INFO);
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);

    printf("\n");
    zwPrintCentered("CHOOSE AN OPERATION:", INFO);
    printf("\n");

    zwMenuItemStyled(1, "Create File", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(2, "Edit File", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(3, "Rename File", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(4, "Delete File", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(5, "File Search (KMP)", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(6, "File Information", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(7, "Zip File (LZ77)", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(8, "Unzip File", getMenuOffset(), PROCESSING_STATEMENTS);
    zwMenuItemStyled(9, "Exit Program", getMenuOffset(), WARNING);

    printf("\n");
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);
    printf("\n");
}

static void menuWithSelection(int selected) {
    int offset = getMenuOffset();

    zwClearScreen();
    printf("\n");
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);
    zwPrintCentered("+-----------------------------------------------------------+", INFO);
    zwPrintCentered("|          ZIP WIZARD - PROFESSIONAL FILE UTILITY          |", SUCCESS);
    zwPrintCentered("+-----------------------------------------------------------+", INFO);
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);

    printf("\n");
    zwPrintCentered("CHOOSE AN OPERATION:", INFO);
    printf("\n");

    for (int i = 1; i <= 9; i++) {
        char line[96];
        const char *label = "";
        int color = PROCESSING_STATEMENTS;

        switch (i) {
            case 1: label = "Create File"; break;
            case 2: label = "Edit File"; break;
            case 3: label = "Rename File"; break;
            case 4: label = "Delete File"; break;
            case 5: label = "File Search (KMP)"; break;
            case 6: label = "File Information"; break;
            case 7: label = "Zip File (LZ77)"; break;
            case 8: label = "Unzip File"; break;
            case 9: label = "Exit Program"; color = WARNING; break;
        }

        if (i == selected) {
            snprintf(line, sizeof(line), ">> [%d] %s", i, label);
            zwPrint(line, offset, SUCCESS);
        } else {
            snprintf(line, sizeof(line), "   [%d] %s", i, label);
            zwPrint(line, offset, color);
        }
    }

    printf("\n");
    zwDrawBoxSimple(getOuterRuleWidth(), PROCESSING_STATEMENTS);
    zwPrintCentered("Use Up/Down arrows and Enter.", INFO);
}

static int readArrowSelection(void) {
    int selected = 1;
    int lastW = -1;
    int lastH = -1;

    menuWithSelection(selected);

    while (1) {
        int w, h;
        getConsoleDimensions(&w, &h);
        if (w != lastW || h != lastH) {
            lastW = w;
            lastH = h;
            menuWithSelection(selected);
        }

        if (_kbhit()) {
            int ch = _getch();
            if (ch == 13) {
                return selected;
            }

            if (ch == 0 || ch == 224) {
                int key = _getch();
                if (key == 72) {
                    selected = (selected == 1) ? 9 : (selected - 1);
                    menuWithSelection(selected);
                } else if (key == 80) {
                    selected = (selected == 9) ? 1 : (selected + 1);
                    menuWithSelection(selected);
                }
            }
        }

        Sleep(40);
    }
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
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Creating a file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Create File", createfile);
        break;
    case 2:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Editing the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Edit File", editfile);
        break;
    case 3:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Renaming the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Rename File", renamefile);
        break;
    case 4:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Deleting the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Delete File", deletefile);
        break;
    case 5:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Searching the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Search File", searchfile);
        break;
    case 6:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Fetching file information...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("File Information", fileinfo);
        break;
    case 7:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Zipping the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Zip File", zipfile);
        break;
    case 8:
        zwPrintCentered("----------------------------------------------------------", INFO);
        zwPrintCentered("Unzipping the file...", PROCESSING_STATEMENTS);
        zwPrintCentered("----------------------------------------------------------", INFO);
        runTimedOperation("Unzip File", unzipFile);
        break;
    }
    printf("\n  Press Enter to return to the main menu...");
    char dummy[10];
    fgets(dummy, sizeof(dummy), stdin);
}

// Define the quitProgram function
void quitProgram() {
    printf("\n");
    zwPrintCentered("===========================================================", INFO);
    zwPrintCentered("Exiting the program...", PROCESSING_STATEMENTS);
    zwPrintCentered("Thank you for using ZIP WIZARD!", SUCCESS);
    if (g_telemetry.totalOps > 0) {
        char summary1[180];
        char summary2[180];
        double avg = g_telemetry.totalMs / (double)g_telemetry.totalOps;
        snprintf(summary1, sizeof(summary1), "Telemetry: total=%lu success=%lu failed=%lu",
            g_telemetry.totalOps, g_telemetry.successOps, g_telemetry.failedOps);
        snprintf(summary2, sizeof(summary2), "Average operation time: %.3f ms", avg);
        zwPrintCentered(summary1, INFO);
        zwPrintCentered(summary2, INFO);
    }
    zwPrintCentered("===========================================================", INFO);
    printf("\n");
    fileIndexShutdown();
    exit(0);
}

// Define the validatechoices function
// Define the validatechoices function
void validatechoices() {
    int choice = readArrowSelection();

    if (choice >= 1 && choice <= 8) {
        userchoice(choice);
        return;
    }

    if (choice == 9) {
        quitProgram();
        return;
    }
}
