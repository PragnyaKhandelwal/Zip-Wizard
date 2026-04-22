#include "fileIndex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define FILE_INDEX_BUCKETS 1024

typedef struct FileIndexNode {
    char *fileName;
    struct FileIndexNode *next;
} FileIndexNode;

static FileIndexNode *g_fileIndex[FILE_INDEX_BUCKETS];
static int g_initialized = 0;

static unsigned int hashFileName(const char *fileName) {
    unsigned int hash = 2166136261u;
    while (*fileName) {
        hash ^= (unsigned char)(*fileName++);
        hash *= 16777619u;
    }
    return hash % FILE_INDEX_BUCKETS;
}

static char *duplicateString(const char *s) {
    size_t len = strlen(s);
    char *copy = (char *)malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, s, len + 1);
    return copy;
}

static void clearIndex(void) {
    for (int i = 0; i < FILE_INDEX_BUCKETS; i++) {
        FileIndexNode *node = g_fileIndex[i];
        while (node) {
            FileIndexNode *next = node->next;
            free(node->fileName);
            free(node);
            node = next;
        }
        g_fileIndex[i] = NULL;
    }
}

void fileIndexInitialize(void) {
    if (g_initialized) {
        clearIndex();
    }

    g_initialized = 1;

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            fileIndexAdd(findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
}

void fileIndexShutdown(void) {
    clearIndex();
    g_initialized = 0;
}

int fileIndexContains(const char *fileName) {
    if (!g_initialized) {
        fileIndexInitialize();
    }

    unsigned int bucket = hashFileName(fileName);
    FileIndexNode *node = g_fileIndex[bucket];

    while (node) {
        if (strcmp(node->fileName, fileName) == 0) {
            return 1;
        }
        node = node->next;
    }

    return 0;
}

void fileIndexAdd(const char *fileName) {
    if (!g_initialized) {
        fileIndexInitialize();
    }

    if (fileIndexContains(fileName)) {
        return;
    }

    unsigned int bucket = hashFileName(fileName);
    FileIndexNode *node = (FileIndexNode *)malloc(sizeof(FileIndexNode));
    if (!node) {
        return;
    }

    node->fileName = duplicateString(fileName);
    if (!node->fileName) {
        free(node);
        return;
    }

    node->next = g_fileIndex[bucket];
    g_fileIndex[bucket] = node;
}

void fileIndexRemove(const char *fileName) {
    if (!g_initialized) {
        fileIndexInitialize();
    }

    unsigned int bucket = hashFileName(fileName);
    FileIndexNode *node = g_fileIndex[bucket];
    FileIndexNode *prev = NULL;

    while (node) {
        if (strcmp(node->fileName, fileName) == 0) {
            if (prev) {
                prev->next = node->next;
            } else {
                g_fileIndex[bucket] = node->next;
            }
            free(node->fileName);
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

void fileIndexRename(const char *oldName, const char *newName) {
    fileIndexRemove(oldName);
    fileIndexAdd(newName);
}

int benchmarkFileIndexLookups(const char *fileName, int iterations) {
    LARGE_INTEGER freq, start, end;
    double indexMs;
    double fsMs;
    int foundCount = 0;

    if (iterations <= 0) {
        iterations = 10000;
    }

    fileIndexInitialize();

    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    for (int i = 0; i < iterations; i++) {
        foundCount += fileIndexContains(fileName);
    }
    QueryPerformanceCounter(&end);
    indexMs = ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)freq.QuadPart;

    QueryPerformanceCounter(&start);
    for (int i = 0; i < iterations; i++) {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(fileName, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            foundCount++;
            FindClose(hFind);
        }
    }
    QueryPerformanceCounter(&end);
    fsMs = ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)freq.QuadPart;

    printf("File Index Benchmark: name=%s iterations=%d\n", fileName, iterations);
    printf("- Hash index lookup time: %.3f ms\n", indexMs);
    printf("- FindFirstFile lookup time: %.3f ms\n", fsMs);
    if (indexMs > 0.0) {
        printf("- Speedup (FindFirstFile/Index): %.3fx\n", fsMs / indexMs);
    }

    (void)foundCount;
    return 0;
}
