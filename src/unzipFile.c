#include "zipFile.h"
#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "unzipFile.h"

typedef struct Lz77Token {
    int offset;
    int length;
    unsigned char nextChar;
} Lz77Token;

static int readExact(FILE *file, void *buffer, size_t size) {
    return fread(buffer, 1, size, file) == size;
}

static void lz77DecompressTokens(const Lz77Token *tokens, size_t tokenCount, FILE *outputFile) {
    unsigned char window[WINDOW_SIZE] = {0};
    int windowPos = 0;

    for (size_t i = 0; i < tokenCount; i++) {
        const Lz77Token *token = &tokens[i];
        if (token->length > 0) {
            int readPos = windowPos - token->offset;
            while (readPos < 0) {
                readPos += WINDOW_SIZE;
            }

            for (int j = 0; j < token->length; j++) {
                unsigned char c = window[readPos % WINDOW_SIZE];
                fputc(c, outputFile);
                window[windowPos % WINDOW_SIZE] = c;
                windowPos++;
                readPos++;
            }
        }

        fputc(token->nextChar, outputFile);
        window[windowPos % WINDOW_SIZE] = token->nextChar;
        windowPos++;
    }
}

static int decompressVersionedArchive(FILE *compressedFile, FILE *outputFile) {
    unsigned char magic[4];
    unsigned char version;
    unsigned char mode;
    uint32_t tokenCount;
    uint64_t rawByteCount;
    uint64_t encodedByteCount;
    HuffmanContext *ctx = NULL;
    unsigned char *encoded = NULL;
    int *decodedSymbols = NULL;
    unsigned char *rawBytes = NULL;
    Lz77Token *tokens = NULL;

    if (!readExact(compressedFile, magic, sizeof(magic)) || memcmp(magic, "ZWZ2", 4) != 0) {
        return 0;
    }

    if (!readExact(compressedFile, &version, sizeof(version)) || version != 2) {
        return 0;
    }

    if (!readExact(compressedFile, &mode, sizeof(mode)) || (mode != 0 && mode != 1 && mode != 2)) {
        return 0;
    }

    if (mode == 2) {
        unsigned char buffer[4096];
        size_t readCount;
        while ((readCount = fread(buffer, 1, sizeof(buffer), compressedFile)) > 0) {
            if (fwrite(buffer, 1, readCount, outputFile) != readCount) {
                return 0;
            }
        }
        return 1;
    }

    if (!readExact(compressedFile, &tokenCount, sizeof(tokenCount))) {
        return 0;
    }

    rawByteCount = (uint64_t)tokenCount * (sizeof(uint16_t) + sizeof(unsigned char) + sizeof(unsigned char));

    if (mode == 0) {
        if (rawByteCount > 0) {
            rawBytes = (unsigned char *)malloc((size_t)rawByteCount);
            if (!rawBytes || !readExact(compressedFile, rawBytes, (size_t)rawByteCount)) {
                free(rawBytes);
                return 0;
            }
        }
    } else {
        if (rawByteCount > 0) {
            ctx = huffmanReadTree(compressedFile);
            if (!ctx) {
                return 0;
            }

            if (!readExact(compressedFile, &encodedByteCount, sizeof(encodedByteCount))) {
                huffmanFreeContext(ctx);
                return 0;
            }

            encoded = (unsigned char *)malloc((size_t)encodedByteCount);
            decodedSymbols = (int *)malloc((size_t)rawByteCount * sizeof(int));
            rawBytes = (unsigned char *)malloc((size_t)rawByteCount);
            if (!encoded || !decodedSymbols || !rawBytes) {
                free(encoded);
                free(decodedSymbols);
                free(rawBytes);
                huffmanFreeContext(ctx);
                return 0;
            }

            if ((encodedByteCount > 0 && !readExact(compressedFile, encoded, (size_t)encodedByteCount)) ||
                huffmanDecode(encoded, (size_t)encodedByteCount, (size_t)rawByteCount, ctx, decodedSymbols) != (int)rawByteCount) {
                free(encoded);
                free(decodedSymbols);
                free(rawBytes);
                huffmanFreeContext(ctx);
                return 0;
            }

            for (uint64_t i = 0; i < rawByteCount; i++) {
                rawBytes[i] = (unsigned char)decodedSymbols[i];
            }
        }
    }

    if (tokenCount > 0) {
        tokens = (Lz77Token *)malloc((size_t)tokenCount * sizeof(Lz77Token));
        if (!tokens || !rawBytes) {
            free(encoded);
            free(decodedSymbols);
            free(rawBytes);
            free(tokens);
            huffmanFreeContext(ctx);
            return 0;
        }

        size_t cursor = 0;
        for (uint32_t i = 0; i < tokenCount; i++) {
            if (cursor + sizeof(uint16_t) + sizeof(unsigned char) + sizeof(unsigned char) > (size_t)rawByteCount) {
                free(encoded);
                free(decodedSymbols);
                free(rawBytes);
                free(tokens);
                huffmanFreeContext(ctx);
                return 0;
            }

            uint16_t packedOffset = (uint16_t)(rawBytes[cursor] | (rawBytes[cursor + 1] << 8));
            cursor += sizeof(uint16_t);
            tokens[i].offset = (int)packedOffset;
            tokens[i].length = (int)rawBytes[cursor++];
            tokens[i].nextChar = rawBytes[cursor++];
        }

        lz77DecompressTokens(tokens, tokenCount, outputFile);
    }

    free(encoded);
    free(decodedSymbols);
    free(rawBytes);
    free(tokens);
    huffmanFreeContext(ctx);

    return 1;
}

void lz77Decompress(FILE *compressedFile, FILE *outputFile) {
    int offset, length;
    unsigned char nextChar;
    unsigned char window[WINDOW_SIZE] = {0};
    int windowPos = 0;

    while (fread(&offset, sizeof(int), 1, compressedFile) == 1 &&
           fread(&length, sizeof(int), 1, compressedFile) == 1 &&
           fread(&nextChar, sizeof(unsigned char), 1, compressedFile) == 1) {

        if (length > 0) {
            int readPos = windowPos - offset;
            while (readPos < 0) {
                readPos += WINDOW_SIZE;
            }

            for (int i = 0; i < length; i++) {
                unsigned char c = window[readPos % WINDOW_SIZE];
                fputc(c, outputFile);
                window[windowPos % WINDOW_SIZE] = c;
                windowPos++;
                readPos++;
            }
        }

        fputc(nextChar, outputFile);
        window[windowPos % WINDOW_SIZE] = nextChar;
        windowPos++;
    }
}

int unzipFilePath(const char *compressedFileName, const char *outputFileName) {
    FILE *compressedFile;
    FILE *outputFile;
    char derivedOutput[MAX_FILE_NAME_LENGTH + 16];
    char *zipExtension;
    char *zwzExtension;
    char err[160];

    if (!compressedFileName || strlen(compressedFileName) == 0) {
        zwPrintAdaptive("Error: compressed file path is empty.", ERROR_FILE);
        return 1;
    }

    if (!zwValidatePath(compressedFileName, err, sizeof(err))) {
        zwPrintAdaptive(err, ERROR_FILE);
        return 1;
    }

    if (!outputFileName || strlen(outputFileName) == 0) {
        zipExtension = strstr(compressedFileName, ".zip");
        zwzExtension = strstr(compressedFileName, ".zwz");
        if (zwzExtension) {
            size_t prefixLength = (size_t)(zwzExtension - compressedFileName);
            snprintf(derivedOutput, sizeof(derivedOutput), "%.*s_output.txt", (int)prefixLength, compressedFileName);
        } else if (zipExtension) {
            size_t prefixLength = (size_t)(zipExtension - compressedFileName);
            snprintf(derivedOutput, sizeof(derivedOutput), "%.*s_output.txt", (int)prefixLength, compressedFileName);
        } else {
            snprintf(derivedOutput, sizeof(derivedOutput), "%s_output.txt", compressedFileName);
        }
        outputFileName = derivedOutput;
    } else if (!zwValidatePath(outputFileName, err, sizeof(err))) {
        zwPrintAdaptive(err, ERROR_FILE);
        return 1;
    }

    compressedFile = fopen(compressedFileName, "rb");
    if (!compressedFile) {
        zwPrintAdaptive("Error: Unable to open compressed file for reading.", ERROR_FILE);
        return 1;
    }

    outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fclose(compressedFile);
        zwPrintAdaptive("Error: Unable to open output file for writing.", ERROR_FILE);
        return 1;
    }

    {
        unsigned char magic[4];
        int headerMatched = 0;
        int usedVersionedFormat = 0;

        if (readExact(compressedFile, magic, sizeof(magic)) && memcmp(magic, "ZWZ2", 4) == 0) {
            headerMatched = 1;
            rewind(compressedFile);
            usedVersionedFormat = decompressVersionedArchive(compressedFile, outputFile);
        }

        if (headerMatched && !usedVersionedFormat) {
            fclose(compressedFile);
            fclose(outputFile);
            zwPrintAdaptive("Error: versioned archive decoding failed.", ERROR_FILE);
            return 1;
        }

        if (!headerMatched) {
            rewind(compressedFile);
            lz77Decompress(compressedFile, outputFile);
        }
    }

    fclose(compressedFile);
    fclose(outputFile);
    zwPrintfAdaptive(INFO, "Unzipped: %s -> %s", compressedFileName, outputFileName);
    return 0;
}

int batchUnzipFiles(int count, const char *files[]) {
    int success = 0;
    int failed = 0;

    if (count <= 0 || !files) {
        return 1;
    }

    for (int i = 0; i < count; i++) {
        int progress = ((i + 1) * 100) / count;
        zwDrawProgressBar(progress, 2, 40, PROCESSING_STATEMENTS);
        zwPrintBlankLine();

        if (unzipFilePath(files[i], NULL) == 0) {
            success++;
        } else {
            failed++;
        }
    }

    zwPrintfAdaptive(INFO, "Batch unzip summary: total=%d success=%d failed=%d", count, success, failed);
    return failed == 0 ? 0 : 1;
}

void unzipFile() {
    char compressedFileName[MAX_PATH * 2];
    char outputFileName[MAX_PATH * 2];

    zwPromptInput("  [UNZIP] Compressed file (.zwz/.zip): ", compressedFileName, sizeof(compressedFileName), INFO);
    zwPromptInput("  [UNZIP] Output file path (Enter for default): ", outputFileName, sizeof(outputFileName), INFO);
    if (unzipFilePath(compressedFileName, strlen(outputFileName) > 0 ? outputFileName : NULL) == 0) {
        zwPrint("File decompressed successfully", 2, SUCCESS);
        zwSetLastOperationStatus(0);
    } else {
        zwSetLastOperationStatus(1);
    }
}
