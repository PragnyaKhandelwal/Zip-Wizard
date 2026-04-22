#ifndef FILEINDEX_H
#define FILEINDEX_H

void fileIndexInitialize(void);
void fileIndexShutdown(void);
int fileIndexContains(const char *fileName);
void fileIndexAdd(const char *fileName);
void fileIndexRemove(const char *fileName);
void fileIndexRename(const char *oldName, const char *newName);
int benchmarkFileIndexLookups(const char *fileName, int iterations);

#endif
