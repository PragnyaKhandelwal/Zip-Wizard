#include "Utils.h"
#include "zipFile.h"
#include "huffman.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

typedef struct Lz77Token {
    int offset;
    int length;
    unsigned char nextChar;
} Lz77Token;

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

static int collectLz77Tokens(
    const unsigned char *data,
    size_t inputSize,
    FindBestMatchFn matcher,
    Lz77Token **outTokens,
    size_t *outTokenCount) {
    int head[HASH_SIZE];
    int *next = NULL;
    Lz77Token *tokens = NULL;
    size_t tokenCount = 0;
    size_t capacity = 64;
    size_t pos = 0;

    if (!outTokens || !outTokenCount) {
        return 0;
    }

    *outTokens = NULL;
    *outTokenCount = 0;

    if (inputSize == 0) {
        return 1;
    }

    memset(head, -1, sizeof(head));

    next = (int *)malloc((inputSize > 0 ? inputSize : 1) * sizeof(int));
    if (!next) {
        return 0;
    }

    for (size_t i = 0; i < inputSize; i++) {
        next[i] = -1;
    }

    tokens = (Lz77Token *)malloc(capacity * sizeof(Lz77Token));
    if (!tokens) {
        free(next);
        return 0;
    }

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
        if (pos + 1 < inputSize) {
            matcher(data, inputSize, pos, head, next, &offset, &length);
        }

        if (tokenCount >= capacity) {
            capacity *= 2;
            Lz77Token *grown = (Lz77Token *)realloc(tokens, capacity * sizeof(Lz77Token));
            if (!grown) {
                free(tokens);
                free(next);
                return 0;
            }
            tokens = grown;
        }

        tokens[tokenCount].offset = offset;
        tokens[tokenCount].length = length;
        tokens[tokenCount].nextChar = data[pos + (size_t)length];
        tokenCount++;

        pos += (size_t)length + 1;
    }

    free(next);
    *outTokens = tokens;
    *outTokenCount = tokenCount;
    return 1;
}

static int writeVersionedArchive(
    FILE *compressedFile,
    const unsigned char *rawBytes,
    size_t rawSize,
    size_t tokenCount,
    const unsigned char *inputBytes,
    size_t inputSize) {
    unsigned int freq[256] = {0};
    int *symbols = NULL;
    unsigned char *encoded = NULL;
    HuffmanContext *ctx = NULL;
    size_t encodedCapacity = rawSize + 64;
    size_t encodedSize = 0;
    uint8_t mode = 0;
    size_t rawPayloadSize = rawSize;
    size_t huffmanPayloadSize = 0;
    size_t literalPayloadSize = inputSize;
    size_t bestPayloadSize;

    for (size_t i = 0; i < rawSize; i++) {
        freq[rawBytes[i]]++;
    }

    if (rawSize > 0) {
        ctx = huffmanBuildTree(freq, 256);
        if (!ctx) {
            return 0;
        }

        symbols = (int *)malloc(rawSize * sizeof(int));
        encoded = (unsigned char *)malloc(encodedCapacity);
        if (!symbols || !encoded) {
            free(symbols);
            free(encoded);
            huffmanFreeContext(ctx);
            return 0;
        }

        for (size_t i = 0; i < rawSize; i++) {
            symbols[i] = rawBytes[i];
        }

        int written = huffmanEncode(symbols, rawSize, ctx, encoded, encodedCapacity);
        if (written < 0) {
            free(symbols);
            free(encoded);
            huffmanFreeContext(ctx);
            return 0;
        }
        encodedSize = (size_t)written;
        huffmanPayloadSize = (256u * sizeof(uint32_t)) + sizeof(uint64_t) + encodedSize;
    }

    bestPayloadSize = rawPayloadSize;
    mode = 0;

    if (rawSize > 0 && huffmanPayloadSize < bestPayloadSize) {
        bestPayloadSize = huffmanPayloadSize;
        mode = 1;
    }

    if (literalPayloadSize < bestPayloadSize) {
        mode = 2;
    }

    if (fwrite("ZWZ2", 1, 4, compressedFile) != 4) {
        free(symbols);
        free(encoded);
        huffmanFreeContext(ctx);
        return 0;
    }

    {
        unsigned char version = 2;
        unsigned char archiveMode = mode;

        if (fwrite(&version, 1, 1, compressedFile) != 1 ||
            fwrite(&archiveMode, 1, 1, compressedFile) != 1) {
            free(symbols);
            free(encoded);
            huffmanFreeContext(ctx);
            return 0;
        }

        if (archiveMode == 0) {
            uint32_t tokenCount32 = (uint32_t)tokenCount;
            if (fwrite(&tokenCount32, sizeof(tokenCount32), 1, compressedFile) != 1 ||
                (rawSize > 0 && fwrite(rawBytes, 1, rawSize, compressedFile) != rawSize)) {
                free(symbols);
                free(encoded);
                huffmanFreeContext(ctx);
                return 0;
            }
        } else if (archiveMode == 1) {
            uint32_t tokenCount32 = (uint32_t)tokenCount;
            uint64_t encodedByteCount = (uint64_t)encodedSize;
            if (fwrite(&tokenCount32, sizeof(tokenCount32), 1, compressedFile) != 1 ||
                fwrite(ctx->frequencies, sizeof(uint32_t), 256, compressedFile) != 256 ||
                fwrite(&encodedByteCount, sizeof(encodedByteCount), 1, compressedFile) != 1 ||
                (encodedSize > 0 && fwrite(encoded, 1, encodedSize, compressedFile) != encodedSize)) {
                free(symbols);
                free(encoded);
                huffmanFreeContext(ctx);
                return 0;
            }
        } else {
            if (inputSize > 0 && fwrite(inputBytes, 1, inputSize, compressedFile) != inputSize) {
                free(symbols);
                free(encoded);
                huffmanFreeContext(ctx);
                return 0;
            }
        }
    }

    free(symbols);
    free(encoded);
    huffmanFreeContext(ctx);
    return 1;
}

static int compressArchiveVersion2(
    const unsigned char *data,
    size_t inputSize,
    FILE *compressedFile,
    FindBestMatchFn matcher,
    CompressionStats *stats) {
    Lz77Token *tokens = NULL;
    size_t tokenCount = 0;
    size_t rawSize = 0;
    unsigned char *rawBytes = NULL;
    int ok;

    ok = collectLz77Tokens(data, inputSize, matcher, &tokens, &tokenCount);
    if (!ok) {
        return 0;
    }

    rawSize = tokenCount * (sizeof(uint16_t) + sizeof(unsigned char) + sizeof(unsigned char));
    if (rawSize > 0) {
        rawBytes = (unsigned char *)malloc(rawSize);
        if (!rawBytes) {
            free(tokens);
            return 0;
        }

        size_t cursor = 0;
        for (size_t i = 0; i < tokenCount; i++) {
            uint16_t packedOffset = (uint16_t)tokens[i].offset;
            unsigned char packedLength = (unsigned char)tokens[i].length;
            rawBytes[cursor++] = (unsigned char)(packedOffset & 0xFFu);
            rawBytes[cursor++] = (unsigned char)((packedOffset >> 8) & 0xFFu);
            rawBytes[cursor++] = packedLength;
            rawBytes[cursor++] = tokens[i].nextChar;
        }
    }

    ok = writeVersionedArchive(compressedFile, rawBytes, rawSize, tokenCount, data, inputSize);
    free(tokens);
    free(rawBytes);

    if (ok) {
        stats->inputBytes = inputSize;
        stats->tokenCount = tokenCount;
    }

    return ok;
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
    CompressionStats stats = {0, 0, 0.0};
    char autoOutput[MAX_PATH * 2];
    const char *target;
    char err[160];

    if (!zwValidatePath(inputFileName, err, sizeof(err))) {
        zwPrintAdaptive(err, ERROR_FILE);
        return 1;
    }

    if (!zwHasTxtExtension(inputFileName)) {
        zwPrintAdaptive("Input file must end with .txt", ERROR_FILE);
        return 1;
    }

    if (!outputZipName || strlen(outputZipName) == 0) {
        snprintf(autoOutput, sizeof(autoOutput), "%s.zwz", inputFileName);
        target = autoOutput;
    } else {
        if (!zwValidatePath(outputZipName, err, sizeof(err))) {
            zwPrintAdaptive(err, ERROR_FILE);
            return 1;
        }
        target = outputZipName;
    }

    inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        zwPrintAdaptive("Error: unable to open input file for zipping.", ERROR_FILE);
        return 1;
    }

    zipOut = fopen(target, "wb");
    if (!zipOut) {
        fclose(inputFile);
        zwPrintAdaptive("Error: unable to open output zip file.", ERROR_FILE);
        return 1;
    }

        unsigned char *inputData = NULL;
        size_t inputSize = readEntireFile(inputFile, &inputData);

        if (!compressArchiveVersion2(inputData, inputSize, zipOut, findBestMatchHashChain, &stats)) {
            fclose(inputFile);
            fclose(zipOut);
            free(inputData);
            zwPrintAdaptive("Error: compression failed.", ERROR_FILE);
            return 1;
        }

        free(inputData);

    fflush(zipOut);
    fclose(inputFile);
    fclose(zipOut);

    {
        FILE *sizeProbe = fopen(target, "rb");
        long outputBytes = 0;
        double ratio = 0.0;

        if (sizeProbe) {
            fseek(sizeProbe, 0, SEEK_END);
            outputBytes = ftell(sizeProbe);
            fclose(sizeProbe);
        }

        if (stats.inputBytes > 0 && outputBytes > 0) {
            ratio = ((double)outputBytes * 100.0) / (double)stats.inputBytes;
        }

        zwPrintfAdaptive(INFO, "Zipped: %s -> %s (Huffman+LZ77 v2)", inputFileName, target);
        zwPrintfAdaptive(INFO, "Compression: tokens=%lu, input=%lu bytes, output=%ld bytes, ratio=%.2f%%, time=%.3f ms",
               (unsigned long)stats.tokenCount,
               (unsigned long)stats.inputBytes,
               outputBytes,
               ratio,
               stats.elapsedMs);
    }

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
        zwPrintBlankLine();

        if (zipFilePath(files[i], NULL) == 0) {
            success++;
        } else {
            failed++;
        }
    }

    zwPrintfAdaptive(INFO, "Batch zip summary: total=%d success=%d failed=%d", count, success, failed);
    return failed == 0 ? 0 : 1;
}

void zipfile() {
    char file_name[MAX_PATH * 2];
    char output_path[MAX_PATH * 2];

    zwPromptInput("  [ZIP] Input file path (.txt): ", file_name, sizeof(file_name), INFO);
    zwPromptInput("  [ZIP] Output file path (.zwz, Enter for default): ", output_path, sizeof(output_path), INFO);

    if (zipFilePath(file_name, strlen(output_path) > 0 ? output_path : NULL) == 0) {
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
        zwPrintfAdaptive(ERROR_FILE, "Error: could not open benchmark input file: %s", inputFileName);
        return 1;
    }

    naiveOut = tmpfile();
    optimizedOut = tmpfile();
    if (!naiveOut || !optimizedOut) {
        zwPrintAdaptive("Error: unable to allocate temporary benchmark outputs.", ERROR_FILE);
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
        zwPrintAdaptive("Error: naive benchmark compression failed.", ERROR_FILE);
        fclose(inputFile);
        fclose(naiveOut);
        fclose(optimizedOut);
        return 1;
    }

    rewind(inputFile);
    if (!runCompressionWithMatcher(inputFile, optimizedOut, findBestMatchHashChain, &optimizedStats)) {
        zwPrintAdaptive("Error: optimized benchmark compression failed.", ERROR_FILE);
        fclose(inputFile);
        fclose(naiveOut);
        fclose(optimizedOut);
        return 1;
    }

    fclose(inputFile);
    fclose(naiveOut);
    fclose(optimizedOut);

    zwPrintfAdaptive(INFO, "Compression Benchmark: %s", inputFileName);
    zwPrintfAdaptive(INFO, "- Naive     : time=%.3f ms, tokens=%lu", naiveStats.elapsedMs, (unsigned long)naiveStats.tokenCount);
    zwPrintfAdaptive(INFO, "- HashChain : time=%.3f ms, tokens=%lu", optimizedStats.elapsedMs, (unsigned long)optimizedStats.tokenCount);

    if (optimizedStats.elapsedMs > 0.0) {
        double speedup = naiveStats.elapsedMs / optimizedStats.elapsedMs;
        snprintf(compareMessage, sizeof(compareMessage), "Speedup (Naive/HashChain): %.3fx", speedup);
        zwPrintfAdaptive(INFO, "- %s", compareMessage);
    }

    return 0;
}

// Huffman+LZ77 integration will be reintroduced after the base compression pipeline is stabilized.
