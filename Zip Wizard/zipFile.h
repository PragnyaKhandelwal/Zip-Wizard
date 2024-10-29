#ifndef ZIPFILE_H
#define ZIPFILE_H

#define WINDOW_SIZE 4096
#define BUFFER_SIZE 18

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
    Node *array[MAX_TREE_NODES];
} MinHeap;

void lz77(FILE *inputFile, FILE *compressedFile);
void huffmanCoding(const char *text, char codes[MAX_TREE_NODES][MAX_TREE_NODES]);
Node* createNode(char character, unsigned frequency);
MinHeap* createMinHeap();
void insertMinHeap(MinHeap* minHeap, Node* node);
Node* extractMin(MinHeap* minHeap);
void buildHuffmanTree(const char *text, char codes[MAX_TREE_NODES][MAX_TREE_NODES]);
void generateHuffmanCodes(Node* root, char *code, int depth, char codes[MAX_TREE_NODES][MAX_TREE_NODES]);
void freeHuffmanTree(Node* root);

int outputCount = 0;
char outputLog[MAX_OUTPUT_LINES][100];

// Function to zip a file
void zipfile();

#endif // ZIPFILE_H
