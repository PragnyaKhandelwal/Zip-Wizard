#include "Utils.h"
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

static int g_lastOperationStatus = 0;
static int getConsoleWidth(void);
static void zwApplyColor(int type);

static int zwGetAdaptiveInputOffset(int width)
{
    int contentWidth = 86;
    if (width <= contentWidth) {
        return 0;
    }
    return (width - contentWidth) / 2;
}

static void zwRenderInputLine(const char *prompt, const char *buffer, int type, SHORT inputRow, int keepCurrentX)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width;
    int startX;
    int offset;
    int promptLen = (int)strlen(prompt);
    int inputLen = (int)strlen(buffer);
    int maxInputWidth;
    const char *visibleInput;
    int visibleLen;
    COORD lineStart;
    DWORD written = 0;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }

    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    startX = csbi.srWindow.Left;
    if (width < 20) {
        width = 20;
    }

    if (keepCurrentX) {
        offset = csbi.dwCursorPosition.X - startX;
        if (offset < 0) {
            offset = 0;
        }
    } else {
        offset = zwGetAdaptiveInputOffset(width);
    }

    maxInputWidth = width - offset - promptLen - 2;
    if (maxInputWidth < 1) {
        maxInputWidth = 1;
    }

    if (inputLen > maxInputWidth) {
        visibleInput = buffer + (inputLen - maxInputWidth);
        visibleLen = maxInputWidth;
    } else {
        visibleInput = buffer;
        visibleLen = inputLen;
    }

    lineStart.X = (SHORT)startX;
    lineStart.Y = inputRow;
    FillConsoleOutputCharacterA(hConsole, ' ', (DWORD)width, lineStart, &written);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, (DWORD)width, lineStart, &written);

    {
        COORD promptPos;
        promptPos.X = (SHORT)(startX + offset);
        promptPos.Y = inputRow;
        SetConsoleCursorPosition(hConsole, promptPos);
    }

    zwApplyColor(type);
    printf("%s", prompt);
    if (visibleLen > 0) {
        printf("%.*s", visibleLen, visibleInput);
    }

    {
        COORD cursorPos;
        cursorPos.X = (SHORT)(startX + offset + promptLen + visibleLen);
        cursorPos.Y = inputRow;
        SetConsoleCursorPosition(hConsole, cursorPos);
    }

    fflush(stdout);
}

static void zwReadLineResponsive(const char *prompt, char *buffer, size_t size, int type)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int lastWidth = -1;
    SHORT inputRow = 0;
    int keepCurrentX = 0;
    size_t len = 0;

    if (!buffer || size == 0) {
        return;
    }

    if (!prompt) {
        prompt = "";
    }

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        buffer[0] = '\0';
        return;
    }

    inputRow = csbi.dwCursorPosition.Y;
    keepCurrentX = (prompt[0] == '\0');

    buffer[0] = '\0';
    zwRenderInputLine(prompt, buffer, type, inputRow, keepCurrentX);

    while (1) {
        int width = getConsoleWidth();
        if (width != lastWidth) {
            lastWidth = width;
            zwRenderInputLine(prompt, buffer, type, inputRow, keepCurrentX);
        }

        if (_kbhit()) {
            int ch = _getch();

            if (ch == 13) {
                printf("\n");
                break;
            }

            if (ch == 8) {
                if (len > 0) {
                    len--;
                    buffer[len] = '\0';
                    zwRenderInputLine(prompt, buffer, type, inputRow, keepCurrentX);
                }
                continue;
            }

            if (ch == 0 || ch == 224) {
                (void)_getch();
                continue;
            }

            if (ch >= 32 && ch <= 126) {
                if (len + 1 < size) {
                    buffer[len++] = (char)ch;
                    buffer[len] = '\0';
                    zwRenderInputLine(prompt, buffer, type, inputRow, keepCurrentX);
                }
                continue;
            }
        }

        Sleep(20);
    }
}

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
    if (!text) {
        printf("\n");
        return;
    }

    int width = getConsoleWidth();
    int available = width - offset - 1;
    if (available < 1) {
        available = 1;
    }

    size_t textLen = strlen(text);
    size_t pos = 0;
    while (pos < textLen) {
        size_t chunk = textLen - pos;
        if ((int)chunk > available) {
            chunk = (size_t)available;
            while (chunk > 0 && text[pos + chunk - 1] != ' ') {
                chunk--;
            }
            if (chunk == 0) {
                chunk = (size_t)available;
            }
        }

        printf("%.*s\n", (int)chunk, text + pos);
        pos += chunk;
        while (pos < textLen && text[pos] == ' ') {
            pos++;
        }
        if (pos < textLen) {
            zwMoveCursor(offset);
        }
    }
}

void zwPrintInline(const char *text, int offset, int type)
{
    zwApplyColor(type);
    zwMoveCursor(offset);
    if (text) {
        int width = getConsoleWidth();
        int available = width - offset - 1;
        if (available < 1) {
            available = 1;
        }
        printf("%.*s", available, text);
    }
    fflush(stdout);
}

void zwReadLine(char *buffer, size_t size, int offset)
{
    zwMoveCursor(offset);
    zwReadLineResponsive("", buffer, size, INFO);
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

    int consoleWidth = getConsoleWidth();
    if (offset < 0) {
        offset = 0;
    }
    if (offset + width > consoleWidth) {
        width = consoleWidth - offset;
    }
    if (width <= 0) {
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
    zwReadLineResponsive(prompt ? prompt : "", buffer, size, type);
    zwApplyColor(INFO);
}

// Input field with visual underline
void zwInputFieldUnderline(const char *prompt, char *buffer, size_t size, int type)
{
    zwApplyColor(type);
    printf("\n");
    
    zwReadLineResponsive(prompt ? prompt : "", buffer, size, type);

    int underlineLen = (int)strlen(prompt ? prompt : "") + 2;
    if (underlineLen > 60) underlineLen = 60;

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
    for (int i = 0; i < width; i++) printf("=");
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
            printf("+");
            for (int x = 1; x < width - 1; x++) printf("-");
            printf("+");
        } else if (y == height - 1) {
            // Bottom border
            printf("+");
            for (int x = 1; x < width - 1; x++) printf("-");
            printf("+");
        } else {
            // Side borders
            printf("|");
            for (int x = 1; x < width - 1; x++) printf(" ");
            printf("|");
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
    for (int i = 0; i < filledWidth; i++) printf("#");
    for (int i = filledWidth; i < width; i++) printf(".");
    printf("] %d%%", percentage);
    fflush(stdout);
    zwApplyColor(INFO);
}

// Styled menu item with number and description
void zwMenuItemStyled(int number, const char *description, int offset, int type)
{
    zwApplyColor(type);
    zwMoveCursor(offset);
    printf("  > [%d] %s\n", number, description);
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

void zwSetLastOperationStatus(int status)
{
    g_lastOperationStatus = status;
}

int zwGetLastOperationStatus(void)
{
    return g_lastOperationStatus;
}

// ===== RESPONSIVE TERMINAL IMPLEMENTATIONS =====

void zwGetConsoleDimensions(int *width, int *height)
{
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

// Wrap and print text to fit terminal width
void zwPrintWrapped(const char *text, int leftMargin, int type)
{
    if (!text) return;
    
    int width, height;
    (void)height;
    zwGetConsoleDimensions(&width, &height);
    
    int maxWidth = width - leftMargin - 2;
    if (maxWidth < 20) maxWidth = 20;
    
    size_t textLen = strlen(text);
    size_t pos = 0;
    
    while (pos < textLen) {
        // Calculate how much text fits on this line
        size_t lineLen = maxWidth;
        if (pos + lineLen > textLen) {
            lineLen = textLen - pos;
        } else {
            // Try to break at a word boundary
            while (lineLen > 0 && text[pos + lineLen - 1] != ' ') {
                lineLen--;
            }
            if (lineLen == 0) {
                lineLen = maxWidth;
            } else {
                // Skip trailing space
                while (lineLen > 0 && text[pos + lineLen - 1] == ' ') {
                    lineLen--;
                }
            }
        }
        
        // Print the line with left margin
        zwApplyColor(type);
        printf("%*s", leftMargin, "");
        printf("%.*s\n", (int)lineLen, text + pos);
        fflush(stdout);
        
        pos += lineLen;
        // Skip any spaces at the start of next line
        while (pos < textLen && text[pos] == ' ') pos++;
    }
    zwApplyColor(INFO);
}

// Print text centered with responsive sizing
void zwPrintCenteredLine(const char *text, int type)
{
    if (!text) return;
    
    int width, height;
    (void)height;
    zwGetConsoleDimensions(&width, &height);
    
    int textLen = (int)strlen(text);
    int pos = (width - textLen) / 2;
    if (pos < 0) pos = 0;
    
    zwPrint(text, pos, type);
}

// Draw responsive horizontal rule
void zwDrawResponsiveRule(char ch, int type)
{
    int width, height;
    (void)height;
    zwGetConsoleDimensions(&width, &height);
    
    int ruleWidth = width - 2;
    if (ruleWidth < 10) ruleWidth = 10;
    
    char line[512];
    if (ruleWidth > (int)(sizeof(line) - 1)) {
        ruleWidth = (int)sizeof(line) - 1;
    }
    
    for (int i = 0; i < ruleWidth; i++) {
        line[i] = ch;
    }
    line[ruleWidth] = '\0';
    
    zwPrintCentered(line, type);
}

// Draw responsive box with title
void zwDrawResponsiveBox(const char *title, int type)
{
    int width, height;
    (void)height;
    zwGetConsoleDimensions(&width, &height);
    
    int boxWidth = width - 4;
    if (boxWidth < 30) boxWidth = 30;
    
    zwDrawResponsiveRule('=', type);
    
    if (title) {
        zwPrintCenteredLine(title, type);
        zwDrawResponsiveRule('=', type);
    }
}

// Print wrapped list of items
void zwPrintWrappedList(const char *items[], int itemCount, int leftMargin, int type)
{
    for (int i = 0; i < itemCount; i++) {
        if (items[i]) {
            zwPrintWrapped(items[i], leftMargin, type);
        }
    }
}

// Draw responsive progress bar
void zwDrawResponsiveProgressBar(const char *label, int percentage, int type)
{
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    
    int width, height;
    (void)height;
    zwGetConsoleDimensions(&width, &height);
    
    int barWidth = width - 25;
    if (barWidth < 10) barWidth = 10;
    
    int leftMargin = (width - barWidth - 20) / 2;
    if (leftMargin < 2) leftMargin = 2;
    
    zwApplyColor(type);
    printf("%*s[%s] ", leftMargin, "", label);
    
    int filledWidth = (barWidth * percentage) / 100;
    for (int i = 0; i < filledWidth; i++) printf("#");
    for (int i = filledWidth; i < barWidth; i++) printf(".");
    printf("] %d%%\n", percentage);
    fflush(stdout);
    zwApplyColor(INFO);
}

int zwHasTxtExtension(const char *fileName)
{
    size_t len;
    if (!fileName)
    {
        return 0;
    }

    len = strlen(fileName);
    if (len < 4)
    {
        return 0;
    }

    return _stricmp(fileName + len - 4, ".txt") == 0;
}

int zwValidateFileName(const char *fileName, char *errorBuf, size_t errorSize)
{
    static const char *reserved[] = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
    };

    size_t len;
    char upperBase[MAX_FILE_NAME_LENGTH + 1];
    size_t i;
    size_t baseLen = 0;

    if (!errorBuf || errorSize == 0)
    {
        return 0;
    }

    errorBuf[0] = '\0';

    if (!fileName)
    {
        snprintf(errorBuf, errorSize, "File name cannot be null.");
        return 0;
    }

    len = strlen(fileName);
    if (len == 0)
    {
        snprintf(errorBuf, errorSize, "File name cannot be empty.");
        return 0;
    }

    if (len > MAX_FILE_NAME_LENGTH)
    {
        snprintf(errorBuf, errorSize, "File name is too long. Max length is %d.", MAX_FILE_NAME_LENGTH);
        return 0;
    }

    if (len >= 2 && fileName[1] == ':')
    {
        snprintf(errorBuf, errorSize, "Use relative file names only (no drive letters).");
        return 0;
    }

    if (strstr(fileName, "..") != NULL)
    {
        snprintf(errorBuf, errorSize, "Parent directory traversal is not allowed.");
        return 0;
    }

    for (i = 0; i < len; i++)
    {
        unsigned char c = (unsigned char)fileName[i];
        if (c < 32)
        {
            snprintf(errorBuf, errorSize, "File name contains non-printable characters.");
            return 0;
        }

        if (strchr("\\/:*?\"<>|", (int)c) != NULL)
        {
            snprintf(errorBuf, errorSize, "File name contains invalid Windows characters.");
            return 0;
        }
    }

    if (fileName[len - 1] == ' ' || fileName[len - 1] == '.')
    {
        snprintf(errorBuf, errorSize, "File name cannot end with space or dot.");
        return 0;
    }

    for (i = 0; i < len && i < sizeof(upperBase) - 1; i++)
    {
        if (fileName[i] == '.')
        {
            break;
        }
        upperBase[baseLen++] = (char)toupper((unsigned char)fileName[i]);
    }
    upperBase[baseLen] = '\0';

    for (i = 0; i < sizeof(reserved) / sizeof(reserved[0]); i++)
    {
        if (strcmp(upperBase, reserved[i]) == 0)
        {
            snprintf(errorBuf, errorSize, "File name is a reserved Windows device name.");
            return 0;
        }
    }

    return 1;
}

int zwValidatePath(const char *path, char *errorBuf, size_t errorSize)
{
    size_t len;
    size_t i;

    if (!errorBuf || errorSize == 0)
    {
        return 0;
    }

    errorBuf[0] = '\0';

    if (!path)
    {
        snprintf(errorBuf, errorSize, "Path cannot be null.");
        return 0;
    }

    len = strlen(path);
    if (len == 0)
    {
        snprintf(errorBuf, errorSize, "Path cannot be empty.");
        return 0;
    }

    if (len >= MAX_PATH * 2)
    {
        snprintf(errorBuf, errorSize, "Path is too long.");
        return 0;
    }

    for (i = 0; i < len; i++)
    {
        unsigned char c = (unsigned char)path[i];
        if (c < 32)
        {
            snprintf(errorBuf, errorSize, "Path contains non-printable characters.");
            return 0;
        }

        if (c == '"' || c == '<' || c == '>' || c == '|')
        {
            snprintf(errorBuf, errorSize, "Path contains invalid Windows characters.");
            return 0;
        }
    }

    return 1;
}
