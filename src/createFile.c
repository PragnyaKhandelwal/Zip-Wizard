#include "createFile.h"
#include "fileIndex.h"
#include "Utils.h"

int createfileNonInteractive(const char *fileName, const char *content)
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

    if (fileIndexContains(fileName))
    {
        zwPrint("File already exists. Choose a different name.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    file = fopen(fileName, "w");
    if (!file)
    {
        zwPrint("Error creating file.", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (content)
    {
        fprintf(file, "%s", content);
    }

    fclose(file);
    fileIndexAdd(fileName);
    zwSetLastOperationStatus(0);
    return 0;
}

void createfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];

    printf("\n");
    zwPromptInput("  [CREATE] File name (.txt): ", file_name, sizeof(file_name), INFO);

    zwPromptInput("  [CONTENT] Initial content: ", file_content, sizeof(file_content), INFO);

    if (createfileNonInteractive(file_name, file_content) == 0)
    {
        zwPrint("  [SUCCESS] Your file has been created successfully!", 2, SUCCESS);
    }
}
