#include "Utils.h"
#include "deleteFile.h"
#include "fileIndex.h"

void deletefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    
    printf("\n");
    zwPromptInput("  [DELETE] File name (.txt): ", file_name, sizeof(file_name), INFO);
    
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("  [ERROR] File name is too long.", 2, ERROR_FILE);
        return;
    }

    if (strlen(file_name) == 0)
    {
        zwPrint("  [ERROR] File name cannot be empty.", 2, ERROR_FILE);
        return;
    }

    if (!fileIndexContains(file_name))
    {
        zwPrint("  [ERROR] File does not exist", 2, ERROR_FILE);
        return;
    }

    if (remove(file_name) == 0)
    {
        fileIndexRemove(file_name);
        zwPrint("  [SUCCESS] Your file has been deleted successfully!", 2, SUCCESS);
    }
    else
    {
        zwPrint("  [ERROR] Error: File does not exist", 2, ERROR_FILE);
    }
}
