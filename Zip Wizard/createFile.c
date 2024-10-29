#include "createFile.h"
#include "Utils.h"

void createfile();

void createfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];

    zwPrint("Enter the name of the file to be created with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);

    // Remove newline character, if present
    file_name[strcspn(file_name, "\n")] = '\0';

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

    // Check if the file name ends with .txt
    if (strstr(file_name, ".txt") == NULL)
    {
        zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR_FILE);
        return;
    }

    // Check if the file already exists
    FILE *checkFile = fopen(file_name, "r");
    if (checkFile)
    {
        fclose(checkFile); // Close the file if it exists

        zwPrint("Error: File already exists. Choose a different name.\n", 20, ERROR_FILE);
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        zwPrint("Error in creating the file\n", 20, ERROR_FILE);
        return;
    }

    zwPrint("Enter the content to be written in the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';
    fprintf(file, "%s", file_content);
    fclose(file);
    zwPrint("Your file has been created successfully!\n", 20, SUCCESS);
}
