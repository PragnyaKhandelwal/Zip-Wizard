#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

#ifndef GetFileExInfoBasic
#define GetFileExInfoBasic 0
#endif

#define MAX_OUTPUT_LINES 50
#define MENU_ITEMS 11
#define INFO 15                  // white
#define PROCESSING_STATEMENTS 11 // cyan
#define WARNING 6                // orange (closest is yellow)
#define ERROR 12                 // red
#define SUCCESS 10               // green
#define MAX_FILE_NAME_LENGTH 99 // maximum length of file name


void menu();
void heading(const char *text);
void zwPrint(const char *text, int offset, int type);
void userchoice(int n);
void createfile();
void editfile();
void renamefile();
void deletefile();
void searchfile();
void fileinfo();
void quitProgram();
int getConsoleWidth();
void reprintOutputs();

int outputCount = 0;
char outputLog[MAX_OUTPUT_LINES][100];

const char *menuItems[MENU_ITEMS] = {
    "** WELCOME TO ZIPWIZARD !! **\n\n\n",
    "The Wizard provides you with various file handling operations:\n",
    "1.) Create File",
    "2.) Edit File",
    "3.) Rename File",
    "4.) Delete File",
    "5.) File Search",
    "6.) File Information",
    "7.) Zip File",
    "8.) Unzip File",
    "9.) Exit"
};


int main()
{
    Sleep(10); // Give time for the console to initialize
    menu();
    return 0;
}

void menu()
{
    int choice = -1;
    int oldWidth = getConsoleWidth(); // Track the previous width
    // Print the initial menu
    system("cls"); // Clear the console once at the beginning
    for (int i = 0; i < MENU_ITEMS; i++)
    {
        if (i == 0)
        {
            heading(menuItems[i]); // Centered heading
        }
        else if (i == 1)
        {
            zwPrint(menuItems[i], 20, INFO); // Menu items with offset
        }
        else
        {
            zwPrint(menuItems[i], 22, INFO);
        }
    }
    zwPrint("", 20, INFO);
    zwPrint("Enter your choice:", 20, INFO); // Prompt for input
    while (1)
    {
        int newWidth = getConsoleWidth();
        // Update if the width has changed
        if (newWidth != oldWidth)
        {
            oldWidth = newWidth;
            system("cls");    // Clear the screen and reprint
            reprintOutputs(); // Reprint all outputs
        }
        // Handle user input
        if (_kbhit())
        {
            char input[10];                     // Buffer for input
            fgets(input, sizeof(input), stdin); // Read input from user
            input[strcspn(input, "\n")] = '\0';
            // clears newline
            //  Try to convert input to an integer
            if (strlen(input) == 0)
            {
                zwPrint("Error: Input cannot be empty. Please enter a number from the menu.\n", 20, WARNING);
                continue; // Go to the next iteration of the loop
            }
            if (sscanf(input, "%d", &choice) == 1)
            {
                if (choice >= 1 && choice <= 6)
                {
                    userchoice(choice);
                }
                else if (choice == 7)
                {
                    quitProgram();
                }
                else
                {
                    zwPrint("Error: Invalid Input!\n", 20, WARNING);
                }
            }
            else
            {
                zwPrint("Error: The input must be an integer\n", 20, ERROR);
            }
            zwPrint("Enter your choice:", 20, INFO); // Prompt again
        }
        Sleep(100); // Slight delay for better user experience
    }
}

void heading(const char *text)
{
    int consoleWidth = getConsoleWidth();
    int textLength = strlen(text);
    int pos = (consoleWidth - textLength) / 2; // Center the text
    zwPrint(text, pos, INFO);
}

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
    case ERROR:
        textColor = ERROR;
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

void reprintOutputs()
{
    // Print the menu again
    heading(menuItems[0]); // Reprint the heading
    for (int i = 1; i < MENU_ITEMS; i++)
    {
        if (i == 1)
        {
            zwPrint(menuItems[i], 20, INFO); // Menu items with offset
        }
        else
        {
            zwPrint(menuItems[i], 22, INFO);
        }
    }
    // Reprint stored outputs
    for (int i = 0; i < outputCount; i++)
    {
        zwPrint(outputLog[i], 20, INFO);
    }
    zwPrint("Enter your choice:", 20, INFO); // Prompt again
}

void userchoice(int n)
{
    switch (n)
    {
    case 1:
        zwPrint("Creating a file...\n", 20, PROCESSING_STATEMENTS);
        createfile();
        break;
    case 2:
        zwPrint("Editing the file...\n", 20, PROCESSING_STATEMENTS);
        editfile();
        break;
    case 3:
        zwPrint("Renaming the file...\n", 20, PROCESSING_STATEMENTS);
        renamefile();
        break;
    case 4:
        zwPrint("Deleting the file...\n", 20, PROCESSING_STATEMENTS);
        deletefile();
        break;
    case 5:
        zwPrint("Searching the file...\n", 20, PROCESSING_STATEMENTS);
        searchfile();
        break;
    case 6:
        zwPrint("Fetching file information...\n", 20, PROCESSING_STATEMENTS);
        fileinfo();
        break;
    case 7:
        zwPrint("Zipping the file...\n", 20, PROCESSING_STATEMENTS);
        break;
    case 8:
        zwPrint("Unzipping the file...\n", 20, PROCESSING_STATEMENTS);
        break;
    }
}

void createfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[1000];

    zwPrint("Enter the name of the file to be created with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);

    // Remove newline character, if present
    file_name[strcspn(file_name, "\n")] = '\0';
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR);
        return;
    }
    if (strlen(file_name) > MAX_FILE_NAME_LENGTH)
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR);
    }

    // Check if the file name ends with .txt
    if (strstr(file_name, ".txt") == NULL)
    {
        zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR);
        return;
    }

    // Check if the file already exists
    FILE *checkFile = fopen(file_name, "r");
    if (checkFile)
    {
        fclose(checkFile); // Close the file if it exists

        zwPrint("Error: File already exists. Choose a different name.\n", 20, ERROR);
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        zwPrint("Error in creating the file\n", 20, ERROR);
        return;
    }

    zwPrint("Enter the content to be written in the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';
    fprintf(file, "%s", file_content);
    fclose(file);
    zwPrint("Your file has been created successfully!\n", 20, SUCCESS);
}

void editfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[1000];
    zwPrint("Enter the name of the file to be edited with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin); // Prevent buffer overflow
    file_name[strcspn(file_name, "\n")] = '\0'; // Clear newline character
    if (strlen(file_name) > MAX_FILE_NAME_LENGTH)
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR);
    }
    if (strstr(file_name, ".txt") == NULL)
    {
        zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR);
        return;
    }
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR);
        return;
    }
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("\n");
        zwPrint("Error: File does not exist.\n", 20, ERROR);
        return;
    }
    printf("\n");
    zwPrint("The current content of the file:", 20, INFO);
    while (fgets(file_content, sizeof(file_content), file))
    {
        printf("%s\n", file_content);
    }
    fclose(file);
    printf("\n");
    zwPrint("Enter the new content to append to the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';
    file = fopen(file_name, "a");
    if (file == NULL)
    {
        zwPrint("Error opening file for editing.\n", 20, ERROR);
        return;
    }
    fprintf(file, "%s\n", file_content);
    fclose(file);
    printf("\n");
    zwPrint("File updated successfully!\n", 20, SUCCESS);
}

void renamefile()
{
    char file_name[100];
    char new_file_name[100];
    zwPrint("Enter the name of the file to be renamed:", 20, INFO);
    scanf("%99s", file_name);
    printf("\n");
    getchar();
    FILE *file = fopen(file_name, "r");
    if (file) {
        zwPrint("Enter the new name of the file:", 20, INFO);
        scanf("%99s", new_file_name);
        printf("\n");
        fclose(file);
    }
     else {
        zwPrint("Error: file does not exist.\n", 20, ERROR);
        return;
    }
    if (rename(file_name, new_file_name) == 0) {
        zwPrint("File renamed successfully.\n", 20, SUCCESS);
    } else {
        zwPrint("Error renaming file\n", 20, ERROR);
    }
}

void deletefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    zwPrint("Enter the name of the file to be deleted with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0';
    if (strlen(file_name) > MAX_FILE_NAME_LENGTH)
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR);
    }
    if (strstr(file_name, ".txt") == NULL)
    {
        zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR);
        return;
    }
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR);
        return;
    }
    if (remove(file_name) == 0)
    {
        printf("\n");
        zwPrint("Your file has been deleted successfully!\n", 20, SUCCESS);
    }
    else
    {
        printf("\n");
        zwPrint("Error: File does not exist\n", 20, ERROR);
    }
}

void searchfile()
{
    char file_name[100];
    zwPrint("Enter the name of the file to be searched:", 20, INFO);
    scanf("%99s", file_name);
    printf("\n");
    getchar();
    FILE *file = fopen(file_name, "r");
    if (file) {
        zwPrint("File found successfully.\n", 20, SUCCESS);
        fclose(file);
    }
    else {
        zwPrint("Error: file does not exist.\n", 20, ERROR);
        return;
    }
}

void fileinfo()
{
    char file_name[100];
    zwPrint("Enter the name of the file:", 20, INFO);
    scanf("%99s", file_name);
    printf("\n");
    getchar();
    FILE *file = fopen(file_name, "r");
    if (file) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    // Get file attributes
    if (GetFileAttributesEx(file_name, GetFileExInfoBasic, &fileInfo)) {
            char fullPath[MAX_PATH];
        if (GetFullPathName(file_name, MAX_PATH, fullPath, NULL) > 0) {
                char output[MAX_PATH + 20]; // Extra space for "File Path: "
            sprintf(output, "File Path: %s", fullPath);
            zwPrint(output, 20, INFO);
        // Prepare output strings
        char attributes[256] = "Attributes: ";
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            strcat(attributes, "Directory ");
        }
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
            strcat(attributes, "Read-Only ");
        }
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) {
            strcat(attributes, "Hidden ");
        }
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
            strcat(attributes, "System ");
        }
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
            strcat(attributes, "Archive ");
        }

        // Print attributes using zwPrint
        zwPrint(attributes, 20, INFO);

        // Retrieve and print timestamps
        FILETIME localCreationTime, localAccessTime, localWriteTime;
        SYSTEMTIME creationTime, accessTime, writeTime;

        // Convert and print creation time
        FileTimeToLocalFileTime(&fileInfo.ftCreationTime, &localCreationTime);
        FileTimeToLocalFileTime(&fileInfo.ftLastAccessTime, &localAccessTime);
        FileTimeToLocalFileTime(&fileInfo.ftLastWriteTime, &localWriteTime);

        FileTimeToSystemTime(&localCreationTime, &creationTime);
        FileTimeToSystemTime(&localAccessTime, &accessTime);
        FileTimeToSystemTime(&localWriteTime, &writeTime);

        char created[256], accessed[256], modified[256];
        sprintf(created, "Created: %02d/%02d/%d %02d:%02d:%02d",
                creationTime.wDay, creationTime.wMonth, creationTime.wYear,
                creationTime.wHour, creationTime.wMinute, creationTime.wSecond);

        sprintf(accessed, "Last Accessed: %02d/%02d/%d %02d:%02d:%02d",
                accessTime.wDay, accessTime.wMonth, accessTime.wYear,
                accessTime.wHour, accessTime.wMinute, accessTime.wSecond);

        sprintf(modified, "Last Modified: %02d/%02d/%d %02d:%02d:%02d\n",
                writeTime.wDay, writeTime.wMonth, writeTime.wYear,
                writeTime.wHour, writeTime.wMinute, writeTime.wSecond);

        // Print timestamps using zwPrint
        zwPrint(created, 20, INFO);
        zwPrint(accessed, 20, INFO);
        zwPrint(modified, 20, INFO);
    }
    }
     else {
        zwPrint("Error retrieving file properties:\n", 20, ERROR);
    }
}
else {
        zwPrint("Error: file does not exist.\n", 20, ERROR);
    }
}

void quitProgram()
{
    printf("\n");
    zwPrint("Exiting the program...\n", 20, PROCESSING_STATEMENTS);
    heading("THANK YOU!!\n");
    exit(0);
}

int getConsoleWidth()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Width of the console
}
