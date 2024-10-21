#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

#define MAX_OUTPUT_LINES 50
#define MENU_ITEMS 8
#define INFO 15  // white
#define PROCESSING_STATEMENTS 11  // cyan
#define WARNING 6  // orange (closest is yellow)
#define ERROR 12  // red
#define SUCCESS 10  // green

void menu();
void heading(const char *text);
void zwPrint(const char *text, int offset, int type);
void userchoice(int n);
void create();
void edit();
void deletefile();
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
    "5.) Zip File",
    "6.) Unzip File",
    "7.) Exit"
};

int main() {
    Sleep(10);  // Give time for the console to initialize
    menu();
    return 0;
}

void menu() {
    int choice = -1;
    int oldWidth = getConsoleWidth();  // Track the previous width

    // Print the initial menu
    system("cls");  // Clear the console once at the beginning
    for (int i = 0; i < MENU_ITEMS; i++) {
        if (i == 0) {
            heading(menuItems[i]);  // Centered heading
        } else if (i == 1) {
            zwPrint(menuItems[i], 20, INFO);  // Menu items with offset
        } else {
            zwPrint(menuItems[i], 22, INFO);
        }
    }
    zwPrint("", 20, INFO);
    zwPrint("Enter your choice:", 20, INFO);  // Prompt for input

    while (1) {
        int newWidth = getConsoleWidth();
        // Update if the width has changed
        if (newWidth != oldWidth) {
            oldWidth = newWidth;
            system("cls");  // Clear the screen and reprint
            reprintOutputs();  // Reprint all outputs
        }

        // Handle user input
        if (_kbhit()) {
            scanf("%d", &choice);
            getchar();  // Clear the newline character from the input buffer
            if (choice >= 1 && choice <= 6) {
                userchoice(choice);
            } else if (choice == 7) {
                quitProgram();
            } else {
                zwPrint("Error: Invalid Input!\n", 20, WARNING);
            }
            zwPrint("Enter your choice:", 20, INFO);  // Prompt again
        }

        Sleep(100);  // Slight delay for better user experience
    }
}

void heading(const char *text) {
    int consoleWidth = getConsoleWidth();
    int textLength = strlen(text);
    int pos = (consoleWidth - textLength) / 2;  // Center the text
    zwPrint(text, pos, INFO);
}

void zwPrint(const char *text, int offset, int type) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int textColor = INFO;  // Default to white
    int bgColor = 0; // Assuming black background

    switch (type) {
        case INFO: textColor = INFO; break;
        case PROCESSING_STATEMENTS: textColor = PROCESSING_STATEMENTS; break;
        case WARNING: textColor = WARNING; break;
        case ERROR: textColor = ERROR; break;
        case SUCCESS: textColor = SUCCESS; break;
    }

    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    COORD coord = {offset, consoleInfo.dwCursorPosition.Y};
    SetConsoleCursorPosition(hConsole, coord);
    printf("%s\n", text);  // Print the text with a newline
}

void reprintOutputs() {
    // Print the menu again
    heading(menuItems[0]);  // Reprint the heading
    for (int i = 1; i < MENU_ITEMS; i++) {
        if (i == 1) {
            zwPrint(menuItems[i], 20, INFO);  // Menu items with offset
        } else {
            zwPrint(menuItems[i], 22, INFO);
        }
    }
    // Reprint stored outputs
    for (int i = 0; i < outputCount; i++) {
        zwPrint(outputLog[i], 20, INFO);
    }
    zwPrint("Enter your choice:", 20, INFO);  // Prompt again
}

void userchoice(int n) {
    switch (n) {
    case 1:
        zwPrint("Creating a file...\n", 20, PROCESSING_STATEMENTS);
        create();
        break;
    case 2:
        zwPrint("Editing the file...\n", 20, PROCESSING_STATEMENTS);
        edit();
        break;
    case 3:
        zwPrint("Renaming the file...\n", 20, PROCESSING_STATEMENTS);
        break;
    case 4:
        zwPrint("Deleting the file...\n", 20, PROCESSING_STATEMENTS);
        deletefile();
        break;
    case 5:
        zwPrint("Zipping the file...\n", 20, PROCESSING_STATEMENTS);
        break;
    case 6:
        zwPrint("Unzipping the file...\n", 20, PROCESSING_STATEMENTS);
        break;
    }
}

void quitProgram() {
    printf("\n");
    zwPrint("Exiting the program...\n", 20, PROCESSING_STATEMENTS);
    heading("THANK YOU!!\n");
    exit(0);
}

void create() {
    char file_name[100];
    char file_content[1000];
    zwPrint("Enter the name of the file to be created with .txt extension:", 20, INFO);
    scanf("%99s", file_name);  // Prevent buffer overflow
    getchar();  // Clear newline character
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        zwPrint("Error in creating the file\n", 20, ERROR);
        return;
    }
    printf("\n");
    zwPrint("Enter the content to be written in the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';  // Remove newline
    fprintf(file, "%s", file_content);
    fclose(file);
    printf("\n");
    zwPrint("Your file has been created successfully!\n", 20, SUCCESS);
}

void edit() {
    char file_name[100];
    char file_content[1000];
    zwPrint("Enter the name of the file to be edited with .txt extension:", 20, INFO);
    scanf("%99s", file_name);  // Prevent buffer overflow
    getchar();  // Clear newline character
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
            printf("\n");
        zwPrint("Error: File does not exist.\n", 20, ERROR);
        return;
    }
    printf("\n");
    zwPrint("The current content of the file:", 20, INFO);
    while (fgets(file_content, sizeof(file_content), file)) {
        printf("%s\n", file_content);
    }
    fclose(file);
    printf("\n");
    zwPrint("Enter the new content to append to the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';
    file = fopen(file_name, "a");
    if (file == NULL) {
        zwPrint("Error opening file for editing.\n", 20, ERROR);
        return;
    }
    fprintf(file, "%s\n", file_content);
    fclose(file);
    printf("\n");
    zwPrint("File updated successfully!\n", 20, SUCCESS);
}

void deletefile() {
    char file_name[100];
    zwPrint("Enter the name of the file to be deleted with .txt extension:", 20, INFO);
    scanf("%99s", file_name);  // Prevent buffer overflow
    if (remove(file_name) == 0) {
            printf("\n");
        zwPrint("Your file has been deleted successfully!\n", 20, SUCCESS);
    } else {
        printf("\n");
        zwPrint("Error: File does not exist\n", 20, ERROR);
    }
}

int getConsoleWidth() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;  // Width of the console
}


