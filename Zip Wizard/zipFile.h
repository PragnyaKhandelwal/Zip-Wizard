#ifndef ZIPFILE_H
#define ZIPFILE_H
#include<stdio.h>
#define MAX_FILE_NAME_LENGTH 255
#define MAX_TREE_NODES 256
#define WINDOW_SIZE 1024
#define BUFFER_SIZE 256

// Node structure for the Huffman tree
typedef struct Node {
    char character;
    unsigned frequency;
    struct Node *left;
    struct Node *right;
} Node;

// Min-heap structure
typedef struct MinHeap {
    unsigned size;
    Node** array;
} MinHeap;

void lz77(FILE *inputFile, FILE *compressedFile);
void huffmanCoding(FILE *lz77Output, const char *compressedFile);
Node* createNode(char character, unsigned frequency);
MinHeap* createMinHeap();
void insertMinHeap(MinHeap* minHeap, Node* node);
Node* extractMin(MinHeap* minHeap);
void buildHuffmanTree(unsigned frequency[], Node** root);
void generateHuffmanCodes(Node* root, char *code, int depth, char codes[MAX_TREE_NODES][MAX_TREE_NODES]);
void freeHuffmanTree(Node* root);
void zipfile();

#endif // ZIPFILE_H
