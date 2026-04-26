#include "renameFile.h"
#include "fileIndex.h"
#include "Utils.h"

int renamefileNonInteractive(const char *oldName, const char *newName)
{
    char err[160];

    if (!zwValidateFileName(oldName, err, sizeof(err)))
    {
        zwPrint(err, 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (!zwValidateFileName(newName, err, sizeof(err)))
    {
        zwPrint(err, 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (!zwHasTxtExtension(oldName) || !zwHasTxtExtension(newName))
    {
        zwPrint("Both file names must end with .txt", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (!fileIndexContains(oldName))
    {
        zwPrint("Source file does not exist.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (fileIndexContains(newName))
    {
        zwPrint("Destination file already exists.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (rename(oldName, newName) != 0)
    {
        zwPrint("Error renaming file", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    fileIndexRename(oldName, newName);
    zwSetLastOperationStatus(0);
    return 0;
}

void renamefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char new_file_name[100];

    zwPromptInput("  [RENAME] Current file name: ", file_name, sizeof(file_name), INFO);
    zwPromptInput("  [RENAME] New file name: ", new_file_name, sizeof(new_file_name), INFO);

    if (renamefileNonInteractive(file_name, new_file_name) == 0)
    {
        zwPrint("File renamed successfully.", 2, SUCCESS);
    }
}
