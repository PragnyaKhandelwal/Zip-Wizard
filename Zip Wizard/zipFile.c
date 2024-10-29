#include "Utils.h"
#include "zipFile.h"

void zipfile();

void zipfile()
{
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char compressed_file[MAX_FILE_NAME_LENGTH + 1];
    zwPrint("Enter the name of the file to be zipped:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline character
    if (!(strlen(file_name) < MAX_FILE_NAME_LENGTH))
    {
        zwPrint("Error: File name is too long.\n", 20, ERROR_FILE);
    }
    if (strstr(file_name, ".txt") == NULL)
    {
        zwPrint("Error: File name must end with .txt extension.\n", 20, ERROR_FILE);
        return;
    }
    if (strlen(file_name) == 0)
    {
        zwPrint("Error: File name cannot be empty.\n", 20, ERROR_FILE);
        return;
    }
    snprintf(compressed_file, sizeof(compressed_file), "%s.huf", file_name);
    FILE *fileName = fopen(file_name, "rb");
    FILE *compressedFile = fopen(compressed_file, "wb");
    lz77(fileName, compressedFile);
    fclose(fileName);
    fclose(compressedFile);
    char codes[MAX_TREE_NODES][MAX_TREE_NODES] = {0};
    huffmanCoding(compressed_file, codes);
    zwPrint("File zipped successfully.\n", 20, SUCCESS);
}

void huffmanCoding(const char *text, char codes[MAX_TREE_NODES][MAX_TREE_NODES])
{
    buildHuffmanTree(text, codes);
}

Node* createNode(char character, unsigned frequency) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->character = character;
    newNode->frequency = frequency;
    newNode->left = newNode->right = NULL;
    return newNode;
}

MinHeap* createMinHeap() {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    return minHeap;
}

void insertMinHeap(MinHeap* minHeap, Node* node) {
    minHeap->array[minHeap->size] = node;
    minHeap->size++;
}

Node* extractMin(MinHeap* minHeap) {
    Node* minNode = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    return minNode;
}

void buildHuffmanTree(const char *text, char codes[MAX_TREE_NODES][MAX_TREE_NODES]) {
    unsigned frequency[MAX_TREE_NODES] = {0};

    // Calculate frequency of each character
    for (const char *p = text; *p; p++) {
        frequency[(unsigned char)*p]++;
    }

    MinHeap* minHeap = createMinHeap();
    for (int i = 0; i < MAX_TREE_NODES; i++) {
        if (frequency[i]) {
            insertMinHeap(minHeap, createNode((char)i, frequency[i]));
        }
    }

    while (minHeap->size > 1) {
        Node* left = extractMin(minHeap);
        Node* right = extractMin(minHeap);

        Node* internalNode = createNode('\0', left->frequency + right->frequency);
        internalNode->left = left;
        internalNode->right = right;

        insertMinHeap(minHeap, internalNode);
    }

    Node* root = extractMin(minHeap);
    char code[MAX_TREE_NODES];
    generateHuffmanCodes(root, code, 0, codes);
    freeHuffmanTree(root);
    free(minHeap);
}

void generateHuffmanCodes(Node* root, char *code, int depth, char codes[MAX_TREE_NODES][MAX_TREE_NODES]) {
    if (root->left) {
        code[depth] = '0';
        generateHuffmanCodes(root->left, code, depth + 1, codes);
    }

    if (root->right) {
        code[depth] = '1';
        generateHuffmanCodes(root->right, code, depth + 1, codes);
    }

    if (!root->left && !root->right) {
        code[depth] = '\0';
        strcpy(codes[(unsigned char)root->character], code);
    }
}

void freeHuffmanTree(Node* root) {
    if (root) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        free(root);
    }
}

void lz77(FILE *inputFile, FILE *compressedFile) {
    char window[WINDOW_SIZE + 1] = {0};
    char buffer[BUFFER_SIZE + 1] = {0};
    int offset, length;
    char nextChar;

    // Read the entire file into memory
    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    fread(fileContent, 1, fileSize, inputFile);
    fileContent[fileSize] = '\0';

    char *current = fileContent;

    while (*current != '\0') {
        // Initialize the window and buffer
        int windowSize = current - fileContent < WINDOW_SIZE ? current - fileContent : WINDOW_SIZE;
        strncpy(window, current - windowSize, windowSize);
        window[windowSize] = '\0';

        // Set the look-ahead buffer
        strncpy(buffer, current, BUFFER_SIZE);
        buffer[BUFFER_SIZE] = '\0';

        offset = 0;
        length = 0;

        // Use a simple search approach (consider hash for efficiency)
        for (int i = 0; i < windowSize; i++) {
            for (int j = 0; j < BUFFER_SIZE && window[i + j] == buffer[j]; j++) {
                if (j > length) {
                    length = j;
                    offset = windowSize - i; // Offset is calculated from the start of the window
                }
            }
        }

        // Determine the next character to output
        nextChar = *(current + length);

        // Write the (offset, length, nextChar) to the compressed file
        fwrite(&offset, sizeof(int), 1, compressedFile);
        fwrite(&length, sizeof(int), 1, compressedFile);
        fwrite(&nextChar, sizeof(char), 1, compressedFile);

        // Move the current pointer forward
        current += length + 1; // Skip over the matched string and include the next character
    }

    free(fileContent);
}
