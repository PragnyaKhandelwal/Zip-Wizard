#include "Utils.h"
#include "deleteFile.h"
#include "fileIndex.h"

void deletefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    zwPrint("Enter the name of the file to be deleted with .txt extension:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0';
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
        return;
    }

    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }

    if (!fileIndexContains(file_name))
    {
        printf("\n");
        zwPrint("Error: File does not exist\n", 20, ERROR_FILE);
        return;
    }

    if (remove(file_name) == 0)
    {
        printf("\n");
        fileIndexRemove(file_name);
        zwPrint("Your file has been deleted successfully!\n", 20, SUCCESS);
    }
    else
    {
        printf("\n");
        zwPrint("Error: File does not exist\n", 20, ERROR_FILE);
    }
}
