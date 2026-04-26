#include "Utils.h"
#include "zipFile.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define HASH_SIZE 4096
#define MIN_MATCH_LENGTH 3
#define MAX_MATCH_LENGTH 255
#define MAX_CHAIN_STEPS 64

typedef struct CompressionStats {
    size_t inputBytes;
    size_t tokenCount;
    double elapsedMs;
} CompressionStats;

typedef void (*FindBestMatchFn)(
    const unsigned char *data,
    size_t inputSize,
    size_t pos,
    const int *head,
    const int *next,
    int *bestOffset,
    int *bestLength);

static int computeHash(const unsigned char *p) {
    return (int)(((unsigned)p[0] * 257u + (unsigned)p[1] * 17u + (unsigned)p[2]) & (HASH_SIZE - 1));
}

static size_t readEntireFile(FILE *inputFile, unsigned char **outData) {
    long fileSize;

    if (fseek(inputFile, 0, SEEK_END) != 0) {
        return 0;
    }

    fileSize = ftell(inputFile);
    if (fileSize < 0) {
        return 0;
    }

    if (fseek(inputFile, 0, SEEK_SET) != 0) {
        return 0;
    }

    if (fileSize == 0) {
        *outData = NULL;
        return 0;
    }

    *outData = (unsigned char *)malloc((size_t)fileSize);
    if (!*outData) {
        return 0;
    }

    if (fread(*outData, 1, (size_t)fileSize, inputFile) != (size_t)fileSize) {
        free(*outData);
        *outData = NULL;
        return 0;
    }

    return (size_t)fileSize;
}

static void findBestMatchNaive(
    const unsigned char *data,
    size_t inputSize,
    size_t pos,
    const int *head,
    const int *next,
    int *bestOffset,
    int *bestLength) {
    (void)head;
    (void)next;

    int localBestOffset = 0;
    int localBestLength = 0;

    if (pos + 1 >= inputSize) {
        *bestOffset = 0;
        *bestLength = 0;
        return;
    }

    size_t windowStart = (pos > WINDOW_SIZE) ? (pos - WINDOW_SIZE) : 0;

    for (size_t candidate = windowStart; candidate < pos; candidate++) {
        int offset = (int)(pos - candidate);
        int maxLen = (int)(inputSize - pos - 1);

        if (maxLen > MAX_MATCH_LENGTH) {
            maxLen = MAX_MATCH_LENGTH;
        }
        if (maxLen > offset) {
            maxLen = offset;
        }

        int length = 0;
        while (length < maxLen && data[candidate + (size_t)length] == data[pos + (size_t)length]) {
            length++;
        }

        if (length > localBestLength) {
            localBestLength = length;
            localBestOffset = offset;
        }
    }

    if (localBestLength < MIN_MATCH_LENGTH) {
        localBestLength = 0;
        localBestOffset = 0;
    }

    *bestOffset = localBestOffset;
    *bestLength = localBestLength;
}

static void findBestMatchHashChain(
    const unsigned char *data,
    size_t inputSize,
    size_t pos,
    const int *head,
    const int *next,
    int *bestOffset,
    int *bestLength) {
    int localBestOffset = 0;
    int localBestLength = 0;

    if (pos + MIN_MATCH_LENGTH >= inputSize) {
        *bestOffset = 0;
        *bestLength = 0;
        return;
    }

    int hash = computeHash(&data[pos]);
    int candidate = head[hash];
    int scanned = 0;
    size_t windowStart = (pos > WINDOW_SIZE) ? (pos - WINDOW_SIZE) : 0;

    while (candidate >= 0 && scanned < MAX_CHAIN_STEPS) {
        if ((size_t)candidate < windowStart) {
            break;
        }

        int offset = (int)(pos - (size_t)candidate);
        int maxLen = (int)(inputSize - pos - 1);

        if (maxLen > MAX_MATCH_LENGTH) {
            maxLen = MAX_MATCH_LENGTH;
        }
        if (maxLen > offset) {
            maxLen = offset;
        }

        int length = 0;
        while (length < maxLen && data[(size_t)candidate + (size_t)length] == data[pos + (size_t)length]) {
            length++;
        }

        if (length > localBestLength) {
            localBestLength = length;
            localBestOffset = offset;
            if (localBestLength == MAX_MATCH_LENGTH) {
                break;
            }
        }

        candidate = next[candidate];
        scanned++;
    }

    if (localBestLength < MIN_MATCH_LENGTH) {
        localBestLength = 0;
        localBestOffset = 0;
    }

    *bestOffset = localBestOffset;
    *bestLength = localBestLength;
}

static int compressData(
    const unsigned char *data,
    size_t inputSize,
    FILE *compressedFile,
    FindBestMatchFn matcher,
    CompressionStats *stats) {
    int head[HASH_SIZE];
    int *next = NULL;
    size_t pos = 0;
    LARGE_INTEGER start, end, frequency;

    memset(head, -1, sizeof(head));

    next = (int *)malloc((inputSize > 0 ? inputSize : 1) * sizeof(int));
    if (!next) {
        return 0;
    }
    for (size_t i = 0; i < inputSize; i++) {
        next[i] = -1;
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    while (pos < inputSize) {
        if (pos > 0 && pos + 1 < inputSize) {
            size_t prev = pos - 1;
            if (prev + 2 < inputSize) {
                int prevHash = computeHash(&data[prev]);
                next[prev] = head[prevHash];
                head[prevHash] = (int)prev;
            }
        }

        int offset = 0;
        int length = 0;
        unsigned char nextChar;

        if (pos + 1 < inputSize) {
            matcher(data, inputSize, pos, head, next, &offset, &length);
        }

        nextChar = data[pos + (size_t)length];

        fwrite(&offset, sizeof(int), 1, compressedFile);
        fwrite(&length, sizeof(int), 1, compressedFile);
        fwrite(&nextChar, sizeof(unsigned char), 1, compressedFile);

        stats->tokenCount++;
        pos += (size_t)length + 1;
    }

    QueryPerformanceCounter(&end);
    stats->elapsedMs = ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)frequency.QuadPart;
    stats->inputBytes = inputSize;

    free(next);
    return 1;
}

static int runCompressionWithMatcher(FILE *inputFile, FILE *outputFile, FindBestMatchFn matcher, CompressionStats *stats) {
    unsigned char *data = NULL;
    size_t inputSize = readEntireFile(inputFile, &data);

    if (inputSize == 0 && data == NULL) {
        stats->inputBytes = 0;
        stats->tokenCount = 0;
        stats->elapsedMs = 0.0;
        return 1;
    }

    stats->inputBytes = 0;
    stats->tokenCount = 0;
    stats->elapsedMs = 0.0;

    int ok = compressData(data, inputSize, outputFile, matcher, stats);
    free(data);
    return ok;
}

int zipFilePath(const char *inputFileName, const char *outputZipName) {
    FILE *inputFile;
    FILE *zipOut;
    CompressionStats stats;
    char autoOutput[MAX_FILE_NAME_LENGTH + 8];
    const char *target;
    char err[160];

    if (!zwValidateFileName(inputFileName, err, sizeof(err))) {
        printf("%s\n", err);
        return 1;
    }

    if (!zwHasTxtExtension(inputFileName)) {
        printf("Input file must end with .txt\n");
        return 1;
    }

    if (!outputZipName || strlen(outputZipName) == 0) {
        snprintf(autoOutput, sizeof(autoOutput), "%s.zip", inputFileName);
        target = autoOutput;
    } else {
        target = outputZipName;
    }

    inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        printf("Error: unable to open input file for zipping.\n");
        return 1;
    }

    zipOut = fopen(target, "wb");
    if (!zipOut) {
        fclose(inputFile);
        printf("Error: unable to open output zip file.\n");
        return 1;
    }

    if (!runCompressionWithMatcher(inputFile, zipOut, findBestMatchHashChain, &stats)) {
        fclose(inputFile);
        fclose(zipOut);
        printf("Error: compression failed.\n");
        return 1;
    }

    fclose(inputFile);
    fclose(zipOut);

    printf("Zipped: %s -> %s\n", inputFileName, target);
    printf("Compression: tokens=%lu, time=%.3f ms\n", (unsigned long)stats.tokenCount, stats.elapsedMs);
    return 0;
}

int batchZipFiles(int count, const char *files[]) {
    int success = 0;
    int failed = 0;

    if (count <= 0 || !files) {
        return 1;
    }

    for (int i = 0; i < count; i++) {
        int progress = ((i + 1) * 100) / count;
        zwDrawProgressBar(progress, 2, 40, PROCESSING_STATEMENTS);
        printf("\n");

        if (zipFilePath(files[i], NULL) == 0) {
            success++;
        } else {
            failed++;
        }
    }

    printf("Batch zip summary: total=%d success=%d failed=%d\n", count, success, failed);
    return failed == 0 ? 0 : 1;
}

void zipfile() {
    char file_name[MAX_FILE_NAME_LENGTH + 1];

    zwPromptInput("  [ZIP] File to zip: ", file_name, sizeof(file_name), INFO);

    if (zipFilePath(file_name, NULL) == 0) {
        zwPrint("File zipped successfully.", 2, SUCCESS);
        zwSetLastOperationStatus(0);
    } else {
        zwSetLastOperationStatus(1);
    }
}

int benchmarkCompressionAlgorithms(const char *inputFileName) {
    FILE *inputFile = fopen(inputFileName, "rb");
    FILE *naiveOut = NULL;
    FILE *optimizedOut = NULL;
    CompressionStats naiveStats;
    CompressionStats optimizedStats;
    char compareMessage[200];

    if (!inputFile) {
        printf("Error: could not open benchmark input file: %s\n", inputFileName);
        return 1;
    }

    naiveOut = tmpfile();
    optimizedOut = tmpfile();
    if (!naiveOut || !optimizedOut) {
        printf("Error: unable to allocate temporary benchmark outputs.\n");
        fclose(inputFile);
        if (naiveOut) {
            fclose(naiveOut);
        }
        if (optimizedOut) {
            fclose(optimizedOut);
        }
        return 1;
    }

    if (!runCompressionWithMatcher(inputFile, naiveOut, findBestMatchNaive, &naiveStats)) {
        printf("Error: naive benchmark compression failed.\n");
        fclose(inputFile);
        fclose(naiveOut);
        fclose(optimizedOut);
        return 1;
    }

    rewind(inputFile);
    if (!runCompressionWithMatcher(inputFile, optimizedOut, findBestMatchHashChain, &optimizedStats)) {
        printf("Error: optimized benchmark compression failed.\n");
        fclose(inputFile);
        fclose(naiveOut);
        fclose(optimizedOut);
        return 1;
    }

    fclose(inputFile);
    fclose(naiveOut);
    fclose(optimizedOut);

    printf("Compression Benchmark: %s\n", inputFileName);
    printf("- Naive     : time=%.3f ms, tokens=%lu\n", naiveStats.elapsedMs, (unsigned long)naiveStats.tokenCount);
    printf("- HashChain : time=%.3f ms, tokens=%lu\n", optimizedStats.elapsedMs, (unsigned long)optimizedStats.tokenCount);

    if (optimizedStats.elapsedMs > 0.0) {
        double speedup = naiveStats.elapsedMs / optimizedStats.elapsedMs;
        snprintf(compareMessage, sizeof(compareMessage), "Speedup (Naive/HashChain): %.3fx", speedup);
        printf("- %s\n", compareMessage);
    }

    return 0;
}
