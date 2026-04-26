#include "huffman.h"
#include <stdlib.h>
#include <string.h>

#define HUFFMAN_SYMBOLS 256

static HuffmanNode *huffmanNodeCreate(int symbol, unsigned int frequency) {
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    if (!node) {
        return NULL;
    }

    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void huffmanFreeTree(HuffmanNode *node) {
    if (!node) {
        return;
    }
    huffmanFreeTree(node->left);
    huffmanFreeTree(node->right);
    free(node);
}

static int huffmanCountSymbols(const unsigned int *freq, int freqTableSize) {
    int count = 0;
    for (int i = 0; i < freqTableSize && i < HUFFMAN_SYMBOLS; i++) {
        if (freq[i] > 0) {
            count++;
        }
    }
    return count;
}

static void huffmanGenerateCodes(HuffmanNode *node, HuffmanCode *codes, uint32_t code, int bits) {
    if (!node) {
        return;
    }

    if (node->symbol >= 0) {
        codes[node->symbol].code = code;
        codes[node->symbol].bits = (bits == 0) ? 1 : bits;
        return;
    }

    huffmanGenerateCodes(node->left, codes, code << 1, bits + 1);
    huffmanGenerateCodes(node->right, codes, (code << 1) | 1u, bits + 1);
}

static HuffmanNode **makeNodeArray(int capacity) {
    return (HuffmanNode **)calloc((size_t)capacity, sizeof(HuffmanNode *));
}

static int insertNodeSorted(HuffmanNode **nodes, int count, HuffmanNode *node) {
    int i = count;
    while (i > 0 && nodes[i - 1]->frequency > node->frequency) {
        nodes[i] = nodes[i - 1];
        i--;
    }
    nodes[i] = node;
    return count + 1;
}

HuffmanContext* huffmanBuildTree(const unsigned int *freq, int freqTableSize) {
    if (!freq || freqTableSize <= 0) {
        return NULL;
    }

    HuffmanContext *ctx = (HuffmanContext *)calloc(1, sizeof(HuffmanContext));
    if (!ctx) {
        return NULL;
    }

    ctx->codes = (HuffmanCode *)calloc(HUFFMAN_SYMBOLS, sizeof(HuffmanCode));
    if (!ctx->codes) {
        free(ctx);
        return NULL;
    }

    memset(ctx->frequencies, 0, sizeof(ctx->frequencies));
    for (int i = 0; i < HUFFMAN_SYMBOLS && i < freqTableSize; i++) {
        ctx->frequencies[i] = freq[i];
    }

    ctx->numSymbols = huffmanCountSymbols(freq, freqTableSize);
    if (ctx->numSymbols == 0) {
        free(ctx->codes);
        free(ctx);
        return NULL;
    }

    if (ctx->numSymbols == 1) {
        for (int i = 0; i < HUFFMAN_SYMBOLS && i < freqTableSize; i++) {
            if (freq[i] > 0) {
                ctx->root = huffmanNodeCreate(i, freq[i]);
                ctx->codes[i].code = 0;
                ctx->codes[i].bits = 1;
                return ctx;
            }
        }
    }

    HuffmanNode **nodes = makeNodeArray(ctx->numSymbols * 2 + 1);
    if (!nodes) {
        free(ctx->codes);
        free(ctx);
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < HUFFMAN_SYMBOLS && i < freqTableSize; i++) {
        if (freq[i] > 0) {
            HuffmanNode *leaf = huffmanNodeCreate(i, freq[i]);
            if (!leaf) {
                for (int j = 0; j < count; j++) {
                    huffmanFreeTree(nodes[j]);
                }
                free(nodes);
                free(ctx->codes);
                free(ctx);
                return NULL;
            }
            count = insertNodeSorted(nodes, count, leaf);
        }
    }

    while (count > 1) {
        HuffmanNode *left = nodes[0];
        HuffmanNode *right = nodes[1];
        HuffmanNode *parent = huffmanNodeCreate(-1, left->frequency + right->frequency);
        if (!parent) {
            for (int i = 0; i < count; i++) {
                huffmanFreeTree(nodes[i]);
            }
            free(nodes);
            free(ctx->codes);
            free(ctx);
            return NULL;
        }

        parent->left = left;
        parent->right = right;

        for (int i = 2; i < count; i++) {
            nodes[i - 2] = nodes[i];
        }
        count -= 2;
        count = insertNodeSorted(nodes, count, parent);
    }

    ctx->root = nodes[0];
    huffmanGenerateCodes(ctx->root, ctx->codes, 0, 0);
    free(nodes);
    return ctx;
}

void huffmanFreeContext(HuffmanContext *ctx) {
    if (!ctx) {
        return;
    }
    huffmanFreeTree(ctx->root);
    free(ctx->codes);
    free(ctx);
}

int huffmanEncode(const int *symbols, size_t count, HuffmanContext *ctx,
                  unsigned char *output, size_t outputSize) {
    if (!symbols || !ctx || !ctx->codes || !output || outputSize == 0) {
        return -1;
    }

    size_t outPos = 0;
    unsigned char currentByte = 0;
    int bitCount = 0;

    for (size_t i = 0; i < count; i++) {
        int sym = symbols[i];
        if (sym < 0 || sym >= HUFFMAN_SYMBOLS) {
            return -1;
        }

        HuffmanCode code = ctx->codes[sym];
        if (code.bits <= 0) {
            return -1;
        }

        for (int bitIndex = code.bits - 1; bitIndex >= 0; bitIndex--) {
            int bit = (int)((code.code >> bitIndex) & 1u);
            currentByte = (unsigned char)((currentByte << 1) | (unsigned char)bit);
            bitCount++;

            if (bitCount == 8) {
                if (outPos >= outputSize) {
                    return -1;
                }
                output[outPos++] = currentByte;
                currentByte = 0;
                bitCount = 0;
            }
        }
    }

    if (bitCount > 0) {
        if (outPos >= outputSize) {
            return -1;
        }
        currentByte = (unsigned char)(currentByte << (8 - bitCount));
        output[outPos++] = currentByte;
    }

    return (int)outPos;
}

int huffmanDecode(const unsigned char *input, size_t inputSize, size_t count,
                  HuffmanContext *ctx, int *output) {
    if (!input || !ctx || !ctx->root || !output) {
        return -1;
    }

    if (ctx->numSymbols == 1) {
        for (size_t i = 0; i < count; i++) {
            output[i] = ctx->root->symbol;
        }
        return (int)count;
    }

    size_t decoded = 0;
    HuffmanNode *node = ctx->root;

    for (size_t byteIndex = 0; byteIndex < inputSize && decoded < count; byteIndex++) {
        unsigned char currentByte = input[byteIndex];
        for (int bitIndex = 7; bitIndex >= 0 && decoded < count; bitIndex--) {
            int bit = (currentByte >> bitIndex) & 1;
            node = (bit == 0) ? node->left : node->right;
            if (!node) {
                return -1;
            }

            if (node->symbol >= 0) {
                output[decoded++] = node->symbol;
                node = ctx->root;
            }
        }
    }

    return (int)decoded;
}

int huffmanWriteTree(HuffmanContext *ctx, FILE *file) {
    if (!ctx || !file || !ctx->codes) {
        return 1;
    }

    if (fwrite(ctx->frequencies, sizeof(uint32_t), HUFFMAN_SYMBOLS, file) != HUFFMAN_SYMBOLS) {
        return 1;
    }

    return 0;
}

HuffmanContext* huffmanReadTree(FILE *file) {
    if (!file) {
        return NULL;
    }

    uint32_t freq[HUFFMAN_SYMBOLS];
    if (fread(freq, sizeof(uint32_t), HUFFMAN_SYMBOLS, file) != HUFFMAN_SYMBOLS) {
        return NULL;
    }

    return huffmanBuildTree(freq, HUFFMAN_SYMBOLS);
}
