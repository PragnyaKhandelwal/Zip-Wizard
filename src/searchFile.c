#include "Utils.h"
#include "searchFile.h"
#include "fileIndex.h"
#include <ctype.h>
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

static int fileContainsKeywordKmpCaseInsensitive(FILE *file, const char *keyword) {
    int keywordLength = (int)strlen(keyword);
    int *lps;
    char loweredKeyword[MAX_KEYWORD_LENGTH];
    char buffer[4096];
    size_t bytesRead;
    int matchedChars = 0;

    if (keywordLength <= 0 || keywordLength >= MAX_KEYWORD_LENGTH) {
        return 0;
    }

    for (int i = 0; i < keywordLength; i++) {
        loweredKeyword[i] = (char)tolower((unsigned char)keyword[i]);
    }
    loweredKeyword[keywordLength] = '\0';

    lps = (int *)malloc((size_t)keywordLength * sizeof(int));
    if (!lps) {
        return 0;
    }

    buildLpsArray(loweredKeyword, keywordLength, lps);

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            char c = (char)tolower((unsigned char)buffer[i]);
            while (matchedChars > 0 && loweredKeyword[matchedChars] != c) {
                matchedChars = lps[matchedChars - 1];
            }

            if (loweredKeyword[matchedChars] == c) {
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

static int wildcardMatch(const char *text, const char *pattern, int caseInsensitive) {
    while (*pattern) {
        if (*pattern == '*') {
            pattern++;
            if (!*pattern) {
                return 1;
            }
            while (*text) {
                if (wildcardMatch(text, pattern, caseInsensitive)) {
                    return 1;
                }
                text++;
            }
            return 0;
        }

        if (*pattern == '?' ||
            (caseInsensitive
                ? (tolower((unsigned char)*pattern) == tolower((unsigned char)*text))
                : (*pattern == *text))) {
            if (!*text) {
                return 0;
            }
            pattern++;
            text++;
            continue;
        }
        return 0;
    }

    return *text == '\0';
}

static int fileContainsWildcardPattern(FILE *file, const char *pattern, int caseInsensitive) {
    char line[4096];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *start = line;
        while (*start) {
            if (wildcardMatch(start, pattern, caseInsensitive)) {
                return 1;
            }
            start++;
        }
    }
    return 0;
}

int searchKeywordAdvanced(const char *keyword, const char *extensionFilter, int caseInsensitive, int useRegex) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char pattern[32];
    int found = 0;
    int tested = 0;

    if (!keyword || strlen(keyword) == 0) {
        printf("Error: keyword cannot be empty.\n");
        return 1;
    }

    if (!extensionFilter || strlen(extensionFilter) == 0) {
        extensionFilter = "txt";
    }

    snprintf(pattern, sizeof(pattern), "*.%s", extensionFilter);
    hFind = FindFirstFile(pattern, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Could not open directory for pattern %s\n", pattern);
        return 1;
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            FILE *file = fopen(findFileData.cFileName, "r");
            tested++;
            if (!file) {
                continue;
            }

            if (useRegex) {
                if (fileContainsWildcardPattern(file, keyword, caseInsensitive)) {
                    printf("MATCH: %s\n", findFileData.cFileName);
                    found++;
                }
            } else if (caseInsensitive) {
                if (fileContainsKeywordKmpCaseInsensitive(file, keyword)) {
                    printf("MATCH: %s\n", findFileData.cFileName);
                    found++;
                }
            } else {
                if (fileContainsKeywordKmp(file, keyword)) {
                    printf("MATCH: %s\n", findFileData.cFileName);
                    found++;
                }
            }

            fclose(file);
        }
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
    printf("Search summary: tested=%d, matched=%d\n", tested, found);
    return 0;
}

void searchfile() {
    char search_option;
    char optionInput[8];
    char file_name[MAX_FILE_NAME_LENGTH], keyword[MAX_KEYWORD_LENGTH];

    // Ask user to choose search type (by name or keyword)
    zwPromptInput("  [SEARCH] Search by name (n) or keyword (k): ", optionInput, sizeof(optionInput), INFO);
    search_option = optionInput[0];

    if (search_option == 'n' || search_option == 'N') {
        // Search by file name
        zwPromptInput("  [SEARCH] File name/pattern: ", file_name, sizeof(file_name), INFO);

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
        char ext[16];
        char ciInput[8];
        char regexInput[8];
        int caseInsensitive = 0;
        int useRegex = 0;

        zwPromptInput("  [SEARCH] Keyword: ", keyword, sizeof(keyword), INFO);

        // Validate keyword input
        if (strlen(keyword) == 0) {
            zwPrint("Error: Keyword cannot be empty.\n", 20, ERROR_FILE);
            return;
        }

        zwPromptInput("  [SEARCH] Extension (default txt): ", ext, sizeof(ext), INFO);
        if (strlen(ext) == 0) {
            strcpy(ext, "txt");
        }

        zwPromptInput("  [SEARCH] Case-insensitive? (y/n): ", ciInput, sizeof(ciInput), INFO);
        if (ciInput[0] == 'y' || ciInput[0] == 'Y') {
            caseInsensitive = 1;
        }

        zwPromptInput("  [SEARCH] Use regex? (y/n): ", regexInput, sizeof(regexInput), INFO);
        if (regexInput[0] == 'y' || regexInput[0] == 'Y') {
            useRegex = 1;
        }

        if (searchKeywordAdvanced(keyword, ext, caseInsensitive, useRegex) != 0) {
            zwPrint("Search failed.", 2, ERROR_FILE);
        }

    } else {
        zwPrint("Invalid option. Choose 'n' for name or 'k' for keyword.\n", 20, ERROR_FILE);
    }
}
