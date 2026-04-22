#include "Utils.h"
#include <string.h>
#include <windows.h>

static void zwApplyColor(int type)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int textColor = INFO;
    int bgColor = 0;

    switch (type)
    {
    case INFO:
        textColor = INFO;
        break;
    case PROCESSING_STATEMENTS:
        textColor = PROCESSING_STATEMENTS;
        break;
    case WARNING:
        textColor = WARNING;
        break;
    case ERROR_FILE:
        textColor = ERROR_FILE;
        break;
    case SUCCESS:
        textColor = SUCCESS;
        break;
    }

    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void zwMoveCursor(int offset)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    COORD coord = {(SHORT)offset, consoleInfo.dwCursorPosition.Y};
    SetConsoleCursorPosition(hConsole, coord);
}

void zwPrint(const char *text, int offset, int type)
{
    zwApplyColor(type);
    zwMoveCursor(offset);
    printf("%s\n", text); // Print the text with a newline
}

void zwPrintInline(const char *text, int offset, int type)
{
    zwApplyColor(type);
    zwMoveCursor(offset);
    printf("%s", text);
    fflush(stdout);
}

void zwReadLine(char *buffer, size_t size, int offset)
{
    zwMoveCursor(offset);
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        if (size > 0)
        {
            buffer[0] = '\0';
        }
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}

void zwClearScreen(void)
{
    system("cls");
}

void zwDrawRule(int offset, int width, char ch, int type)
{
    if (width <= 0)
    {
        return;
    }

    char line[256];
    if (width >= (int)sizeof(line))
    {
        width = (int)sizeof(line) - 1;
    }

    for (int i = 0; i < width; i++)
    {
        line[i] = ch;
    }
    line[width] = '\0';
    zwPrint(line, offset, type);
}

void terminalSize(int width, int height) {
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set the window size
    SMALL_RECT windowSize = { 0, 0, width - 1, height - 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // Get the console window handle
    HWND hWnd = GetConsoleWindow();

    // Disable resizing
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    style &= ~WS_SIZEBOX; // Remove the resize box
    style &= ~WS_MAXIMIZEBOX; // Remove maximize button
    SetWindowLong(hWnd, GWL_STYLE, style);

    // Set the window size
    SetWindowPos(hWnd, NULL, 0, 0, width * 8, height * 16, SWP_NOZORDER | SWP_NOMOVE);
}

// ===== NEW PROFESSIONAL UI IMPLEMENTATIONS =====

// Get console width for centering
static int getConsoleWidth(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

// Centered text printing
void zwPrintCentered(const char *text, int type)
{
    int consoleWidth = getConsoleWidth();
    int textLength = (int)strlen(text);
    int pos = (consoleWidth - textLength) / 2;
    if (pos < 0) pos = 0;
    zwPrint(text, pos, type);
}

// Prompt + Input aligned properly (KEY IMPROVEMENT)
void zwPromptInput(const char *prompt, char *buffer, size_t size, int type)
{
    zwApplyColor(type);
    
    // Print prompt at column 0 and read from where it ends
    printf("%s", prompt);
    fflush(stdout);
    
    // Read input on the same line
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        if (size > 0) buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    
    // Reset color
    zwApplyColor(INFO);
}

// Input field with visual underline
void zwInputFieldUnderline(const char *prompt, char *buffer, size_t size, int type)
{
    zwApplyColor(type);
    printf("\n  %s", prompt);
    fflush(stdout);
    
    // Calculate underline width
    int underlineLen = (int)strlen(prompt) + 2;
    if (underlineLen > 60) underlineLen = 60;
    
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        if (size > 0) buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    
    // Print underline below input
    printf("  ");
    for (int i = 0; i < underlineLen; i++) printf("_");
    printf("\n");
    fflush(stdout);
    
    zwApplyColor(INFO);
}

// Simple box drawing (top border)
void zwDrawBoxSimple(int width, int type)
{
    zwApplyColor(type);
    printf("  ");
    for (int i = 0; i < width; i++) printf("═");
    printf("\n");
    fflush(stdout);
    zwApplyColor(INFO);
}

// Complex box drawing
void zwDrawBox(int width, int height, int offsetX, int offsetY, int type)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    zwApplyColor(type);
    
    // Each row
    for (int y = 0; y < height; y++) {
        COORD coord = {(SHORT)offsetX, (SHORT)(offsetY + y)};
        SetConsoleCursorPosition(hConsole, coord);
        
        if (y == 0) {
            // Top border
            printf("╔");
            for (int x = 1; x < width - 1; x++) printf("═");
            printf("╗");
        } else if (y == height - 1) {
            // Bottom border
            printf("╚");
            for (int x = 1; x < width - 1; x++) printf("═");
            printf("╝");
        } else {
            // Side borders
            printf("║");
            for (int x = 1; x < width - 1; x++) printf(" ");
            printf("║");
        }
    }
    fflush(stdout);
    zwApplyColor(INFO);
}

// Progress bar display
void zwDrawProgressBar(int percentage, int offsetX, int width, int type)
{
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    
    zwApplyColor(type);
    zwMoveCursor(offsetX);
    
    printf("[");
    int filledWidth = (width * percentage) / 100;
    for (int i = 0; i < filledWidth; i++) printf("█");
    for (int i = filledWidth; i < width; i++) printf("░");
    printf("] %d%%", percentage);
    fflush(stdout);
    zwApplyColor(INFO);
}

// Styled menu item with number and description
void zwMenuItemStyled(int number, const char *description, int offset, int type)
{
    zwApplyColor(type);
    zwMoveCursor(offset);
    printf("  ► [%d] %s\n", number, description);
    fflush(stdout);
    zwApplyColor(INFO);
}

// Clear a line at specific Y position
void zwClearLine(int offsetY)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, (SHORT)offsetY};
    SetConsoleCursorPosition(hConsole, coord);
    
    int consoleWidth = getConsoleWidth();
    for (int i = 0; i < consoleWidth; i++) {
        printf(" ");
    }
    SetConsoleCursorPosition(hConsole, coord);
}
