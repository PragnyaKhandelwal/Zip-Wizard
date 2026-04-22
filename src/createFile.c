#include "createFile.h"
#include "fileIndex.h"
#include "Utils.h"

void createfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];

    printf("\n");
    zwPromptInput("  [CREATE] File name (.txt): ", file_name, sizeof(file_name), INFO);

    // Check if the file name is empty
    if (strlen(file_name) == 0)
    {
        zwPrint("  [ERROR] File name cannot be empty.", 2, ERROR_FILE);
        return;
    }

    // Check if the file name is too long
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("  [ERROR] File name is too long. Maximum length is 99 characters.", 2, ERROR_FILE);
        return;
    }
    // Check if the file already exists (hash index lookup)
    if (fileIndexContains(file_name))
    {
        zwPrint("  [ERROR] File already exists. Choose a different name.", 2, ERROR_FILE);
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        zwPrint("  [ERROR] Error in creating the file", 2, ERROR_FILE);
        return;
    }

    zwPromptInput("  [CONTENT] Initial content: ", file_content, sizeof(file_content), INFO);
    fprintf(file, "%s", file_content);
    fclose(file);
    fileIndexAdd(file_name);
    zwPrint("  [SUCCESS] Your file has been created successfully!", 2, SUCCESS);
}
