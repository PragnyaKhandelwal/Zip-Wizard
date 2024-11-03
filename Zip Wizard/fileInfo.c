#include "Utils.h"
#include "fileInfo.h"

#ifndef GetFileExInfoBasic
#define GetFileExInfoBasic 0
#endif

void fileinfo()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    zwPrint("Enter the name of the file to view its info:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline character
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
    }
   
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }
    // Construct the full file path
    char file_path[MAX_FILE_NAME_LENGTH + 2];
    snprintf(file_path, sizeof(file_path), "%s", file_name);
    printf("Checking file path: %s\n", file_path); // Debugging output

    // Attempt to open the file
    FILE *file = fopen(file_path, "r");
    if (file)
    {
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        if (GetFileAttributesEx(file_path, GetFileExInfoBasic, &fileInfo))
        {
            char fullPath[MAX_PATH];
            if (GetFullPathName(file_path, MAX_PATH, fullPath, NULL) > 0)
            {
                zwPrint(fullPath, 20, INFO);
            }

            // Prepare output strings for attributes
            char attributes[256] = "Attributes: ";
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                strcat(attributes, "Directory ");
            }
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
            {
                strcat(attributes, "Read-Only ");
            }
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            {
                strcat(attributes, "Hidden ");
            }
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            {
                strcat(attributes, "System ");
            }
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
            {
                strcat(attributes, "Archive ");
            }

            // Print attributes
            zwPrint(attributes, 20, INFO);

            // Retrieve and print timestamps
            FILETIME localCreationTime, localAccessTime, localWriteTime;
            SYSTEMTIME creationTime, accessTime, writeTime;

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

            sprintf(modified, "Last Modified: %02d/%02d/%d %02d:%02d:%02d",
                    writeTime.wDay, writeTime.wMonth, writeTime.wYear,
                    writeTime.wHour, writeTime.wMinute, writeTime.wSecond);

            // Print timestamps
            zwPrint(created, 20, INFO);
            zwPrint(accessed, 20, INFO);
            zwPrint(modified, 20, INFO);
        }
        fclose(file);
    }
    else
    {
        zwPrint("Error: file does not exist or could not be opened.\n", 20, ERROR_FILE);
    }
}
