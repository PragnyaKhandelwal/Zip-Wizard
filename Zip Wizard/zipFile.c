#include "Utils.h"
#include "zipFile.h"

void zipfile() {
    char file_name[MAX_FILE_NAME_LENGTH + 1];
    char compressed_file[MAX_FILE_NAME_LENGTH + 1];

    zwPrint("Enter the name of the file to be zipped:", 20, INFO);
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline character

    snprintf(compressed_file, sizeof(compressed_file), "%s.zip", file_name);
    FILE *fileName = fopen(file_name, "rb");
    FILE *zipFile = fopen(compressed_file, "wb");

    if (!fileName || !zipFile) {
        zwPrint("Error: Unable to open files for zipping.\n", 20, ERROR_FILE);
        if (fileName) fclose(fileName);
        return;
    }

    // Step 5: Perform LZ77 compression
    lz77(fileName, zipFile);
    fclose(fileName);
    rewind(zipFile); // Prepare to read from the start of the zip file

    // Step 6: Perform Huffman coding on LZ77 output
    huffmanCoding(zipFile, compressed_file);

    fclose(zipFile);
    zwPrint("File zipped successfully.\n", 20, SUCCESS);
}

void lz77(FILE *inputFile, FILE *compressedFile) {
    char window[WINDOW_SIZE] = {0};
    char buffer[BUFFER_SIZE + 1] = {0};
    int offset, length;
    char nextChar;

    int windowStart = 0;
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, inputFile)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        char *current = buffer;

        while (*current) {
            int maxMatchLen = 0;
            offset = 0;

            // Search for matches in the sliding window
            for (int i = 0; i < windowStart && *current; i++) {
                int j = 0;
                while (window[i + j] == *current && j < BUFFER_SIZE && window[i + j] != '\0') {
                    j++;
                }
                if (j > maxMatchLen) {
                    maxMatchLen = j;
                    offset = windowStart - i;
                }
            }

            // Determine the next character to output
            nextChar = *(current + maxMatchLen);
            length = maxMatchLen;

            // Write (offset, length, nextChar) to the compressed file
            fwrite(&offset, sizeof(int), 1, compressedFile);
            fwrite(&length, sizeof(int), 1, compressedFile);
            fwrite(&nextChar, sizeof(char), 1, compressedFile);

            // Update the sliding window
            if (windowStart + length + 1 <= WINDOW_SIZE) {
                strncpy(window + windowStart, current, length + 1);
                windowStart += length + 1;
            } else {
                int shift = length + 1;
                memmove(window, window + shift, windowStart - shift);
                windowStart -= shift;
                strncpy(window + windowStart, current, length + 1);
                windowStart += length + 1;
            }

            current += length + 1; // Move to the next character
        }
    }
}

void huffmanCoding(FILE *lz77Output, const char *zipFileName) {
    unsigned frequency[MAX_TREE_NODES] = {0};
    int offset, length;
    char nextChar;

    // Build frequency table from LZ77 output
    while (fread(&offset, sizeof(int), 1, lz77Output) == 1 &&
           fread(&length, sizeof(int), 1, lz77Output) == 1 &&
           fread(&nextChar, sizeof(char), 1, lz77Output) == 1) {
        frequency[(unsigned char)nextChar]++;
    }

    // Build Huffman Tree
    Node* root = NULL;
    buildHuffmanTree(frequency, &root);

    // Generate Huffman codes
    char codes[MAX_TREE_NODES][MAX_TREE_NODES] = {0};
    char code[MAX_TREE_NODES];
    generateHuffmanCodes(root, code, 0, codes);

    // Reset LZ77 output for encoding
    rewind(lz77Output);

    // Prepare to write encoded data to zipFile
    FILE *zipFile = fopen(zipFileName, "ab"); // Append to the zip file
    if (!zipFile) {
        zwPrint("Error: Unable to open zip file for writing.\n", 20, ERROR_FILE);
        return;
    }
    // Step 10: Encode LZ77 output using Huffman codes
    while (fread(&offset, sizeof(int), 1, lz77Output) == 1 &&
           fread(&length, sizeof(int), 1, lz77Output) == 1 &&
           fread(&nextChar, sizeof(char), 1, lz77Output) == 1) {
        // Write encoded data to zipFile
        fwrite(&offset, sizeof(int), 1, zipFile);
        fwrite(&length, sizeof(int), 1, zipFile);

        // Debug: Check nextChar before encoding
        if (nextChar < MAX_TREE_NODES) {
            zwPrint("Encoding character: %d using Huffman code\n", (unsigned char)nextChar, INFO);
            fwrite(codes[(unsigned char)nextChar], sizeof(char), strlen(codes[(unsigned char)nextChar]), zipFile);
        } else {
            zwPrint("Warning: Character %d has no Huffman code\n", (unsigned char)nextChar, WARNING);
        }
    }
    fclose(zipFile);
    // Free the Huffman tree
    freeHuffmanTree(root);
}

void buildHuffmanTree(unsigned frequency[], Node** root) {
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
    *root = extractMin(minHeap);
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
    minHeap->array = (Node**)malloc(MAX_TREE_NODES * sizeof(Node*)); // This line will work now
    return minHeap;
}

void insertMinHeap(MinHeap* minHeap, Node* node) {
    minHeap->array[minHeap->size++] = node;
}

Node* extractMin(MinHeap* minHeap) {
    Node* minNode = minHeap->array[0];
    minHeap->array[0] = minHeap->array[--minHeap->size];
    return minNode;
}
