#include "createFile.h"
#include "fileIndex.h"
#include "Utils.h"

void createfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];

    zwPrintInline("File name (.txt): ", 20, INFO);
    zwReadLine(file_name, sizeof(file_name), 38);

    // Check if the file name is empty
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }

    // Check if the file name is too long
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long. Maximum length is 99 characters.\n", 20, ERROR_FILE);
        return;
    }
    // Check if the file already exists (hash index lookup)
    if (fileIndexContains(file_name))
    {
        zwPrint("Error: File already exists. Choose a different name.\n", 20, ERROR_FILE);
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        zwPrint("Error in creating the file\n", 20, ERROR_FILE);
        return;
    }

    zwPrintInline("Initial content: ", 20, INFO);
    zwReadLine(file_content, sizeof(file_content), 37);
    fprintf(file, "%s", file_content);
    fclose(file);
    fileIndexAdd(file_name);
    zwPrint("Your file has been created successfully!\n", 20, SUCCESS);
}
