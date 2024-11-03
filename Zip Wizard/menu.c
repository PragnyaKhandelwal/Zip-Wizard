#include "Utils.h"
#include "menu.h"
#include "createFile.h"
#include "editFile.h"
#include "renameFile.h"
#include "deleteFile.h"
#include "searchFile.h"
#include "fileInfo.h"
#include "zipFile.h"
#include "unzipFile.h"

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
    "9.) Exit"};

void menu()
{
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
    printf("\n");
    zwPrint("Enter your choice:", 20, INFO); // Prompt for input
}
    void validatechoices()
    {
        int choice = -1;
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
             // Go to the next iteration of the loop
            }
            else if (sscanf(input, "%d", &choice) == 1)
            {
                if (choice >= 1 && choice <= 8)
                {
                    userchoice(choice);
                }
                else if (choice == 9)
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
                zwPrint("Error: The input must be an integer\n", 20, ERROR_FILE);
            }
            zwPrint("Enter your choice:", 20, INFO); // Prompt again
        }
    }

void heading(const char *text)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD bufferSize = {100, 100}; // Buffer size must be at least as large as the window size
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1; // Width of the console
    // Calculate the position to center the text
    int textLength = strlen(text);
    int pos = (consoleWidth - textLength) / 2;
    zwPrint(text, pos, INFO);
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
        zipfile();
        break;
    case 8:
        zwPrint("Unzipping the file...\n", 20, PROCESSING_STATEMENTS);
        //unzipfile();
        break;
    }
}

void quitProgram()
{
    printf("\n");
    zwPrint("Exiting the program...\n", 20, PROCESSING_STATEMENTS);
    heading("THANK YOU!!\n");
    exit(0);
}
