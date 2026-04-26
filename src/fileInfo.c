#include "Utils.h"
#include "fileInfo.h"
#include "fileIndex.h"

#ifndef GetFileExInfoBasic
#define GetFileExInfoBasic 0
#endif

int fileinfoNonInteractive(const char *fileName)
{
    char file_path[MAX_FILE_NAME_LENGTH + 2];
    char err[160];

    if (!zwValidateFileName(fileName, err, sizeof(err)))
    {
        zwPrintAdaptive(err, ERROR_FILE);
        return 1;
    }

    snprintf(file_path, sizeof(file_path), "%s", fileName);
    if (!fileIndexContains(file_path))
    {
        zwPrintAdaptive("Error: file does not exist or could not be opened.", ERROR_FILE);
        return 1;
    }

    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        zwPrintAdaptive("Error: file does not exist or could not be opened.", ERROR_FILE);
        return 1;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesEx(file_path, GetFileExInfoBasic, &fileInfo))
    {
        char fullPath[MAX_PATH];
        char attributes[256] = "Attributes: ";
        FILETIME localCreationTime, localAccessTime, localWriteTime;
        SYSTEMTIME creationTime, accessTime, writeTime;

        if (GetFullPathName(file_path, MAX_PATH, fullPath, NULL) > 0)
        {
            zwPrintAdaptive(fullPath, INFO);
        }

        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) strcat(attributes, "Directory ");
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) strcat(attributes, "Read-Only ");
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) strcat(attributes, "Hidden ");
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) strcat(attributes, "System ");
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) strcat(attributes, "Archive ");
        zwPrintAdaptive(attributes, INFO);

        FileTimeToLocalFileTime(&fileInfo.ftCreationTime, &localCreationTime);
        FileTimeToLocalFileTime(&fileInfo.ftLastAccessTime, &localAccessTime);
        FileTimeToLocalFileTime(&fileInfo.ftLastWriteTime, &localWriteTime);
        FileTimeToSystemTime(&localCreationTime, &creationTime);
        FileTimeToSystemTime(&localAccessTime, &accessTime);
        FileTimeToSystemTime(&localWriteTime, &writeTime);

        zwPrintfAdaptive(INFO, "Created: %02d/%02d/%d %02d:%02d:%02d",
            creationTime.wDay, creationTime.wMonth, creationTime.wYear,
            creationTime.wHour, creationTime.wMinute, creationTime.wSecond);
        zwPrintfAdaptive(INFO, "Last Accessed: %02d/%02d/%d %02d:%02d:%02d",
            accessTime.wDay, accessTime.wMonth, accessTime.wYear,
            accessTime.wHour, accessTime.wMinute, accessTime.wSecond);
        zwPrintfAdaptive(INFO, "Last Modified: %02d/%02d/%d %02d:%02d:%02d",
            writeTime.wDay, writeTime.wMonth, writeTime.wYear,
            writeTime.wHour, writeTime.wMinute, writeTime.wSecond);
    }

    fclose(file);
    return 0;
}

void fileinfo()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    zwPromptInput("  [INFO] File name for info: ", file_name, sizeof(file_name), INFO);

    if (fileinfoNonInteractive(file_name) != 0)
    {
        zwSetLastOperationStatus(1);
        return;
    }

    zwSetLastOperationStatus(0);
}
