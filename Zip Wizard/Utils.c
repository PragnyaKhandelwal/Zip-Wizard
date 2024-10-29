#include "Utils.h"
#include <windows.h>

void zwPrint(const char *text, int offset, int type);

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
