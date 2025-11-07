#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include<stdint.h>

#define MAX_TREE_HEIGHT 256

typedef struct MinHeapNode {
	unsigned char data;
	unsigned freq;
	struct MinHeapNode *left, *right;
} MinHeapNode;

typedef struct MinHeap {
	unsigned int size;
	unsigned int capacity;
	MinHeapNode** array;
} MinHeap;

MinHeapNode* newNode(unsigned char data, unsigned int freq);
MinHeap* createMinHeap(unsigned int capacity);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
MinHeapNode* extractMin(MinHeap* minHeap);
MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size);
void freeHuffmanTree(MinHeapNode* root);

#endif
