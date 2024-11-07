#include "Utils.h"
#include "searchFile.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_FILE_NAME_LENGTH 100
#define MAX_KEYWORD_LENGTH 100

void searchfile() {
    char search_option;
    char file_name[MAX_FILE_NAME_LENGTH], keyword[MAX_KEYWORD_LENGTH];

    // Ask user to choose search type (by name or keyword)
    zwPrint("Search by (n)ame or by (k)eyword?", 20, INFO);
    search_option = getchar();
    getchar();  // Consume newline character left by getchar()

    if (search_option == 'n' || search_option == 'N') {
        // Search by file name
        zwPrint("Enter the name of the file to be searched with .txt:\n", 20, INFO);
        fgets(file_name, sizeof(file_name), stdin);
        file_name[strcspn(file_name, "\n")] = '\0';  // Remove newline

        // Validate file name 
       if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
    }

    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }

        // Windows-specific file search with pattern matching
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(file_name, &findFileData); 

        if (hFind == INVALID_HANDLE_VALUE) {
            zwPrint("Error: Could not find files matching the pattern.\n", 20, ERROR_FILE);
            return;
        }

        int found = 0;
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  
                zwPrint("File found succesfully!\n", 20, SUCCESS);
                    found = 1;  // At least one file was found
            }
        } while (FindNextFile(hFind, &findFileData));  // Continue with next file

        FindClose(hFind);

        if (!found) {
            zwPrint("No files found matching the pattern.\n", 20, ERROR_FILE);
        }

    } else if (search_option == 'k' || search_option == 'K') {
        // Search by keyword in text files
        zwPrint("Enter the keyword to search in text files:", 20, INFO);
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';  // Remove newline

        // Validate keyword input
        if (strlen(keyword) == 0) {
            zwPrint("Error: Keyword cannot be empty.\n", 20, ERROR_FILE);
            return;
        }

        // Windows-specific code to search for files
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile("*.txt", &findFileData);  // Search for text files (*.txt)

        if (hFind == INVALID_HANDLE_VALUE) {
            zwPrint("Error: Could not open directory to search for .txt files.\n", 20, ERROR_FILE);
            return;
        }

        int found = 0;
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  
                // Open the file for reading
                FILE *file = fopen(findFileData.cFileName, "r");
                if (file) {
                    char line[1024];
                    while (fgets(line, sizeof(line), file)) {
                        if (strstr(line, keyword)) {  // Keyword match
                            zwPrint("Found keyword in file:", 20, SUCCESS);
                            printf("%s\n",findFileData.cFileName) ;// Display the file name where keyword was found
                            found = 1;
                            break;
                        }
                    }
                    fclose(file);
                } else {
                    zwPrint("Warning: Could not open file %s for reading.\n", 20, WARNING);  // Warning if file can't be opened
                }
            }
        } while (FindNextFile(hFind, &findFileData));  // Continue with next file

        FindClose(hFind);

        if (!found) {
            zwPrint("No matching files found containing the keyword.\n", 20, ERROR_FILE);
        }

    } else {
        zwPrint("Invalid option. Choose 'n' for name or 'k' for keyword.\n", 20, ERROR_FILE);
    }
}
