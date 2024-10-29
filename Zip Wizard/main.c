#include <windows.h>
#include <conio.h>
#include "Utils.h"
#include "createFile.h"
#include "editFile.h"
#include "renameFile.h"
#include "deleteFile.h"
#include "searchFile.h"
#include "fileInfo.h"
#include "zipFile.h"
#include "unzipFile.h"

#define MENU_ITEMS 11

void menu();
void heading(const char *text);
void userchoice(int n);
void quitProgram();
int getConsoleWidth();
void reprintOutputs();

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

int getConsoleWidth()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Width of the console
}

