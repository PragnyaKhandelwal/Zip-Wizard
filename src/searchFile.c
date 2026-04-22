#include "Utils.h"
#include "searchFile.h"
#include "fileIndex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define MAX_KEYWORD_LENGTH 100

static int hasWildcard(const char *pattern) {
    return strchr(pattern, '*') != NULL || strchr(pattern, '?') != NULL;
}

static void buildLpsArray(const char *pattern, int patternLength, int *lps) {
    int length = 0;
    lps[0] = 0;

    for (int i = 1; i < patternLength;) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else if (length != 0) {
            length = lps[length - 1];
        } else {
            lps[i] = 0;
            i++;
        }
    }
}

static int fileContainsKeywordKmp(FILE *file, const char *keyword) {
    const int keywordLength = (int)strlen(keyword);
    if (keywordLength == 0) {
        return 0;
    }

    int *lps = (int *)malloc((size_t)keywordLength * sizeof(int));
    if (!lps) {
        return 0;
    }

    buildLpsArray(keyword, keywordLength, lps);

    char buffer[4096];
    size_t bytesRead;
    int matchedChars = 0;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            while (matchedChars > 0 && keyword[matchedChars] != buffer[i]) {
                matchedChars = lps[matchedChars - 1];
            }

            if (keyword[matchedChars] == buffer[i]) {
                matchedChars++;
                if (matchedChars == keywordLength) {
                    free(lps);
                    return 1;
                }
            }
        }
    }

    free(lps);
    return 0;
}

void searchfile() {
    char search_option;
    char optionInput[8];
    char file_name[MAX_FILE_NAME_LENGTH], keyword[MAX_KEYWORD_LENGTH];

    // Ask user to choose search type (by name or keyword)
    zwPrintInline("Search by name(n) or keyword(k): ", 20, INFO);
    zwReadLine(optionInput, sizeof(optionInput), 53);
    search_option = optionInput[0];

    if (search_option == 'n' || search_option == 'N') {
        // Search by file name
        zwPrintInline("File name/pattern: ", 20, INFO);
        zwReadLine(file_name, sizeof(file_name), 39);

        // Validate file name 
       if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
        return;
    }

    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }

        if (!hasWildcard(file_name)) {
            if (fileIndexContains(file_name)) {
                zwPrint("File found succesfully!\n", 20, SUCCESS);
            } else {
                zwPrint("Error: Could not find files matching the pattern.\n", 20, ERROR_FILE);
            }
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
        zwPrintInline("Keyword: ", 20, INFO);
        zwReadLine(keyword, sizeof(keyword), 29);

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
                    if (fileContainsKeywordKmp(file, keyword)) {
                        zwPrint("Found keyword in file:", 20, SUCCESS);
                        printf("%s\n", findFileData.cFileName); // Display the file name where keyword was found
                        found = 1;
                    }
                    fclose(file);
                } else {
                    char warningMessage[160];
                    snprintf(warningMessage, sizeof(warningMessage), "Warning: Could not open file %s for reading.", findFileData.cFileName);
                    zwPrint(warningMessage, 20, WARNING);  // Warning if file can't be opened
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
