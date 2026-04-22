#include "Utils.h"
#include "menu.h"
#include "zipFile.h"
#include "fileIndex.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc == 4 && strcmp(argv[1], "--benchmark-index") == 0)
    {
        return benchmarkFileIndexLookups(argv[2], atoi(argv[3]));
    }

    if (argc == 3 && strcmp(argv[1], "--benchmark-compress") == 0)
    {
        return benchmarkCompressionAlgorithms(argv[2]);
    }

    fileIndexInitialize();
    terminalSize(110,35);
    menu();
    while (1)
    {
        validatechoices();
    }
    return 0;
}
