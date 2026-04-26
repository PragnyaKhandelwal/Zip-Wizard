#include <stdio.h>
#include "Utils.h"
#include "editFile.h"

int editfileNonInteractive(const char *fileName, const char *appendContent)
{
    FILE *file;
    char err[160];

    if (!zwValidateFileName(fileName, err, sizeof(err)))
    {
        zwPrint(err, 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (!zwHasTxtExtension(fileName))
    {
        zwPrint("File name must end with .txt", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    file = fopen(fileName, "a");
    if (!file)
    {
        zwPrint("Error opening file for editing.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (appendContent)
    {
        fprintf(file, "%s\n", appendContent);
    }

    fclose(file);
    zwSetLastOperationStatus(0);
    return 0;
}

void editfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];
    char err[160];

    zwPromptInput("  [EDIT] File to edit (.txt): ", file_name, sizeof(file_name), INFO);

    if (!zwValidateFileName(file_name, err, sizeof(err)))
    {
        zwPrint(err, 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return;
    }

    if (!zwHasTxtExtension(file_name))
    {
        zwPrint("File name must end with .txt", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return;
    }

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        zwPrint("Error: File does not exist.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return;
    }
    printf("\n");
    zwPrint("Current content of the file:", 2, INFO);
    while (fgets(file_content, sizeof(file_content), file))
    {
        printf("%s\n", file_content);
    }
    fclose(file);
    printf("\n");
    zwPromptInput("  [EDIT] Append content: ", file_content, sizeof(file_content), INFO);

    if (editfileNonInteractive(file_name, file_content) == 0)
    {
        zwPrint("File updated successfully!", 2, SUCCESS);
    }
}
