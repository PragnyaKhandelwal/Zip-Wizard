#include "Utils.h"
#include <windows.h>

void zwPrint(const char *text, int offset, int type)
{
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int textColor = INFO; // Default to white
    int bgColor = 0;      // Assuming black background
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
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    COORD coord = {offset, consoleInfo.dwCursorPosition.Y};
    SetConsoleCursorPosition(hConsole, coord);
    printf("%s\n", text); // Print the text with a newline
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
