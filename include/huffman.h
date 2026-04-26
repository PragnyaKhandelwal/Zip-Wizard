#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Huffman tree node
typedef struct HuffmanNode {
    int symbol;                // -1 for internal nodes, 0-255 for bytes
    unsigned int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

// Huffman code for a byte symbol
typedef struct HuffmanCode {
    uint32_t code;
    int bits;
} HuffmanCode;

// Huffman encoder/decoder context
typedef struct HuffmanContext {
    HuffmanCode *codes;        // Huffman codes for each byte [0..255]
    HuffmanNode *root;         // Root of Huffman tree
    int numSymbols;            // Number of unique byte values
    unsigned int frequencies[256];
} HuffmanContext;

// Build Huffman tree from frequency table and generate codes.
// freqTableSize must be at least 256.
HuffmanContext* huffmanBuildTree(const unsigned int *freq, int freqTableSize);

// Free Huffman context and its tree
void huffmanFreeContext(HuffmanContext *ctx);

// Encode a byte stream using Huffman codes.
int huffmanEncode(const int *symbols, size_t count, HuffmanContext *ctx,
                  unsigned char *output, size_t outputSize);

// Decode a Huffman-encoded stream.
int huffmanDecode(const unsigned char *input, size_t inputSize, size_t count,
                  HuffmanContext *ctx, int *output);

// Write Huffman frequency table to file.
int huffmanWriteTree(HuffmanContext *ctx, FILE *file);

// Read Huffman frequency table from file and rebuild the tree.
HuffmanContext* huffmanReadTree(FILE *file);

#endif
