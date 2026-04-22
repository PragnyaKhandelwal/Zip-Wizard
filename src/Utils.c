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
