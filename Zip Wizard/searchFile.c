#include "Utils.h"
#include "searchFile.h"

void searchfile();

void searchfile()
{
    char file_name[100];
    zwPrint("Enter the name of the file to be searched:", 20, INFO);
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
    if (file)
    {
        zwPrint("File found successfully.\n", 20, SUCCESS);
        fclose(file);
    }
    else
    {
        zwPrint("Error: file does not exist.\n", 20, ERROR_FILE);
        return;
    }
}
