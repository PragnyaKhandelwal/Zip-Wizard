#include "Utils.h"
#include "menu.h"
#include "zipFile.h"
#include "unzipFile.h"
#include "createFile.h"
#include "editFile.h"
#include "renameFile.h"
#include "deleteFile.h"
#include "searchFile.h"
#include "fileInfo.h"
#include "fileIndex.h"
#include <string.h>
#include <stdlib.h>

static void printUsage(void)
{
    zwPrintAdaptive("Zip Wizard Usage:", INFO);
    zwPrintAdaptive("  Interactive mode: zipwizard.exe", INFO);
    zwPrintAdaptive("  create: zipwizard.exe --create <file.txt> <content>", INFO);
    zwPrintAdaptive("  edit: zipwizard.exe --edit <file.txt> <append-content>", INFO);
    zwPrintAdaptive("  rename: zipwizard.exe --rename <old.txt> <new.txt>", INFO);
    zwPrintAdaptive("  delete: zipwizard.exe --delete <file.txt>", INFO);
    zwPrintAdaptive("  search: zipwizard.exe --search --keyword <kw> [--ext txt] [--ignore-case] [--regex]", INFO);
    zwPrintAdaptive("  info: zipwizard.exe --info <file.txt>", INFO);
    zwPrintAdaptive("  zip: zipwizard.exe --zip <file.txt> [output.zwz]", INFO);
    zwPrintAdaptive("  unzip: zipwizard.exe --unzip <file.zwz|file.zip> [output.txt]", INFO);
    zwPrintAdaptive("  batch zip: zipwizard.exe --batch-zip <file1.txt> <file2.txt> ...", INFO);
    zwPrintAdaptive("  batch unzip: zipwizard.exe --batch-unzip <file1.zwz> <file2.zwz> ...", INFO);
    zwPrintAdaptive("  benchmark index: zipwizard.exe --benchmark-index <file> <iterations>", INFO);
    zwPrintAdaptive("  benchmark compress: zipwizard.exe --benchmark-compress <file>", INFO);
}

int main(int argc, char *argv[])
{
    fileIndexInitialize();

    if (argc == 4 && strcmp(argv[1], "--benchmark-index") == 0)
    {
        return benchmarkFileIndexLookups(argv[2], atoi(argv[3]));
    }

    if (argc == 3 && strcmp(argv[1], "--benchmark-compress") == 0)
    {
        return benchmarkCompressionAlgorithms(argv[2]);
    }

    if (argc >= 2 && strcmp(argv[1], "--create") == 0)
    {
        if (argc < 4) { printUsage(); return 1; }
        return createfileNonInteractive(argv[2], argv[3]);
    }

    if (argc >= 2 && strcmp(argv[1], "--edit") == 0)
    {
        if (argc < 4) { printUsage(); return 1; }
        return editfileNonInteractive(argv[2], argv[3]);
    }

    if (argc >= 2 && strcmp(argv[1], "--rename") == 0)
    {
        if (argc < 4) { printUsage(); return 1; }
        return renamefileNonInteractive(argv[2], argv[3]);
    }

    if (argc >= 2 && strcmp(argv[1], "--delete") == 0)
    {
        if (argc < 3) { printUsage(); return 1; }
        return deletefileNonInteractive(argv[2]);
    }

    if (argc >= 2 && strcmp(argv[1], "--info") == 0)
    {
        if (argc < 3) { printUsage(); return 1; }
        return fileinfoNonInteractive(argv[2]);
    }

    if (argc >= 2 && strcmp(argv[1], "--zip") == 0)
    {
        if (argc < 3) { printUsage(); return 1; }
        return zipFilePath(argv[2], argc >= 4 ? argv[3] : NULL);
    }

    if (argc >= 2 && strcmp(argv[1], "--unzip") == 0)
    {
        if (argc < 3) { printUsage(); return 1; }
        return unzipFilePath(argv[2], argc >= 4 ? argv[3] : NULL);
    }

    if (argc >= 3 && strcmp(argv[1], "--batch-zip") == 0)
    {
        return batchZipFiles(argc - 2, (const char **)&argv[2]);
    }

    if (argc >= 3 && strcmp(argv[1], "--batch-unzip") == 0)
    {
        return batchUnzipFiles(argc - 2, (const char **)&argv[2]);
    }

    if (argc >= 2 && strcmp(argv[1], "--search") == 0)
    {
        const char *keyword = NULL;
        const char *ext = "txt";
        int caseInsensitive = 0;
        int useRegex = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--keyword") == 0 && i + 1 < argc) {
                keyword = argv[++i];
            } else if (strcmp(argv[i], "--ext") == 0 && i + 1 < argc) {
                ext = argv[++i];
            } else if (strcmp(argv[i], "--ignore-case") == 0) {
                caseInsensitive = 1;
            } else if (strcmp(argv[i], "--regex") == 0) {
                useRegex = 1;
            }
        }

        if (!keyword) {
            printUsage();
            return 1;
        }

        return searchKeywordAdvanced(keyword, ext, caseInsensitive, useRegex);
    }

    if (argc > 1)
    {
        printUsage();
        return 1;
    }

    while (1)
    {
        menu();
        validatechoices();
    }
    return 0;
}
