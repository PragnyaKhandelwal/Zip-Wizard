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

// Define the menu function
void menu() {
    const char *menuItems[] = {
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

    // Print the menu items
    for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++) {
        if (i == 0) {
            heading(menuItems[i]);
        } else {
            zwPrint(menuItems[i], 22, INFO);
        }
    }
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
    int textLength = strlen(text);
    int pos = (consoleWidth - textLength) / 2;
    zwPrint(text, pos, INFO);
}

// Define the userchoice function
void userchoice(int n) {
    switch (n) {
    case 1:
        zwPrint("Creating a file...\n", 20, PROCESSING_STATEMENTS);
        createfile();
        while (getchar() != '\n'); 
        break;
    case 2:
        zwPrint("Editing the file...\n", 20, PROCESSING_STATEMENTS);
        editfile();
        while (getchar() != '\n'); 
        break;
    case 3:
        zwPrint("Renaming the file...\n", 20, PROCESSING_STATEMENTS);
        renamefile();
        while (getchar() != '\n'); 
        break;
    case 4:
        zwPrint("Deleting the file...\n", 20, PROCESSING_STATEMENTS);
        deletefile();
        while (getchar() != '\n'); 
        break;
    case 5:
        zwPrint("Searching the file...\n", 20, PROCESSING_STATEMENTS);
        searchfile();
        while (getchar() != '\n'); 
        break;
    case 6:
        zwPrint("Fetching file information...\n", 20, PROCESSING_STATEMENTS);
        fileinfo();
        while (getchar() != '\n'); 
        break;
    case 7:
        zwPrint("Zipping the file...\n", 20, PROCESSING_STATEMENTS);
        zipfile();
        while (getchar() != '\n'); 
        break;
    case 8:
        zwPrint("Unzipping the file...\n", 20, PROCESSING_STATEMENTS);
        unzipFile();
        while (getchar() != '\n'); 
        break;
    }
}

// Define the quitProgram function
void quitProgram() {
    printf("\n");
    zwPrint("Exiting the program...\n", 20, PROCESSING_STATEMENTS);
    heading("THANK YOU!!\n");
    exit(0);  // Make sure to include stdlib.h for exit()
}

// Define the validatechoices function
// Define the validatechoices function
void validatechoices() {
    int choice = -1;

    // Prompt for input
    zwPrint("Enter your choice:", 20, INFO);

    // Continuously prompt the user until a valid input is entered
    while (1) {
        char input[10];  // Buffer for input
        fgets(input, sizeof(input), stdin);  // Read input from user

        // Remove newline character if it exists
        input[strcspn(input, "\n")] = '\0';  // Clears newline character

        // Check for empty input
        if (strlen(input) == 0) {
            zwPrint("Error: Input cannot be empty. Please enter a number from the menu.\n", 20, WARNING);
            continue;  // Skip this iteration and prompt again
        }

        // Try to convert input to an integer
        if (sscanf(input, "%d", &choice) != 1) {
            zwPrint("Error: The input must be an integer.\n", 20, ERROR_FILE);
            continue;  // Skip this iteration and prompt again
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
            zwPrint("Error: Invalid Input! Please enter a number between 1 and 9.\n", 20, WARNING);
        }
    }
}
