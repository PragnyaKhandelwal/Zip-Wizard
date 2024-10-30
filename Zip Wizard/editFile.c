#include <stdio.h>
#include "Utils.h"
#include "editFile.h"

void editfile();

void editfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char file_content[MAX_CONTENT_LENGTH + 1];
    zwPrint("Enter the name of the file to be edited with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin); // Prevent buffer overflow
    file_name[strcspn(file_name, "\n")] = '\0'; // Clear newline character
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
    }
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("\n");
        zwPrint("Error: File does not exist.\n", 20, ERROR_FILE);
        return;
    }
    printf("\n");
    zwPrint("The current content of the file:", 20, INFO);
    while (fgets(file_content, sizeof(file_content), file))
    {
        printf("%s\n", file_content);
    }
    fclose(file);
    printf("\n");
    zwPrint("Enter the new content to append to the file:", 20, INFO);
    fgets(file_content, sizeof(file_content), stdin);
    file_content[strcspn(file_content, "\n")] = '\0';
    file = fopen(file_name, "a");
    if (file == NULL)
    {
        zwPrint("Error opening file for editing.\n", 20, ERROR_FILE);
        return;
    }
    fprintf(file, "%s\n", file_content);
    fclose(file);
    printf("\n");
    zwPrint("File updated successfully!\n", 20, SUCCESS);
}
