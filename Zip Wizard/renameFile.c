#include "renameFile.h"
#include "Utils.h"

void renamefile();

void renamefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char new_file_name[100];
    zwPrint("Enter the name of the file to be renamed:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
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
    if (file)
    {
        zwPrint("Enter the new name of the file:", 20, INFO);

        fgets(new_file_name, sizeof(new_file_name), stdin);
        file_name[strcspn(new_file_name, "\n")] = '\0'; // Clear newline character
        if (strlen(new_file_name) > MAX_FILE_NAME_LENGTH)
        {
            zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
        }
        if (strstr(new_file_name, ".txt") == NULL)
        {
            zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR_FILE);
            return;
        }
        if (strlen(new_file_name) == 0)
        {
            zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
            return;
        }
    }
    else
    {
        zwPrint("Error: file does not exist.\n", 20, ERROR_FILE);
        return;
    }
    if (rename(file_name, new_file_name) == 0)
    {
        zwPrint("File renamed successfully.\n", 20, SUCCESS);
    }
    else
    {
        zwPrint("Error renaming file\n", 20, ERROR_FILE);
    }
}
