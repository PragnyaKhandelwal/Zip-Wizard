#include "Utils.h"
#include "deleteFile.h"
#include "fileIndex.h"

int deletefileNonInteractive(const char *fileName)
{
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

    if (!fileIndexContains(fileName))
    {
        zwPrint("File does not exist", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    if (remove(fileName) != 0)
    {
        zwPrint("Error deleting file", 2, ERROR_FILE);
        zwSetLastOperationStatus(1);
        return 1;
    }

    fileIndexRemove(fileName);
    zwSetLastOperationStatus(0);
    return 0;
}

void deletefile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    
    printf("\n");
    zwPromptInput("  [DELETE] File name (.txt): ", file_name, sizeof(file_name), INFO);
    
    if (deletefileNonInteractive(file_name) == 0)
    {
        zwPrint("  [SUCCESS] Your file has been deleted successfully!", 2, SUCCESS);
    }
}
