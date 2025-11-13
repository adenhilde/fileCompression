#include "huffman_tree.h"
#include<stdlib.h>

MinHeapNode* newNode(unsigned char data, unsigned int freq) {
	MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
	temp->left = temp->right = NULL;
	temp->data = data;
	temp->freq = freq;
	return temp;
}

MinHeap* createMinHeap(unsigned int capacity) {
	MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
	return minHeap;
}

static void swapMinHeapNode(MinHeapNode** x, MinHeapNode** y){
	MinHeapNode* temp = *x;
	*x = *y;
	*y = temp;
}

static void minHeapify(MinHeap* minHeap, int index) {
	int smallest = index;
	int left = 2*index + 1;
	int right = 2*index + 2;

	if(left < (int)minHeap->size && minHeap->array [left]->freq < minHeap->array[smallest]->freq)
		smallest = left;

	if(right < (int)minHeap->size && minHeap->array [right]->freq < minHeap->array[smallest]->freq)
		smallest = right;

	if (smallest != index) {
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[index]);
		minHeapify(minHeap, smallest);
	}
}

MinHeapNode* extractMin(MinHeap* minHeap) {
	MinHeapNode* temp = minHeap->array[0];
	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	--minHeap->size;
	minHeapify(minHeap, 0);
	return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
	++minHeap->size;
	int i = minHeap->size - 1;

	while(i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq){
		minHeap->array[i] = minHeap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	minHeap->array[i] = minHeapNode;
}

static void buildMinHeap(MinHeap* minHeap) {
	int n = minHeap->size - 1;
	for (int i = (n - 1) / 2; i >=0; --i)
		minHeapify(minHeap, i);
}

static MinHeap* createAndBuildMinHeap(unsigned char data[], int freq[], int size) {
	MinHeap* minHeap = createMinHeap(size);
	for (int i = 0; i < size; i++)
		minHeap->array[i] = newNode(data[i], freq[i]);
	minHeap->size = size;
	buildMinHeap(minHeap);
	return minHeap;
}

MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size) {
	MinHeapNode *left, *right, *top;
	MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

	while(minHeap->size != 1){
		left = extractMin(minHeap);
		right = extractMin(minHeap);
		top = newNode('$', left->freq + right->freq);
		top->left = left;
		top->right = right;
		insertMinHeap(minHeap, top);
	}

	MinHeapNode* root = extractMin(minHeap);
	free(minHeap->array);
	free(minHeap);
	return root;
}



void freeHuffmanTree(MinHeapNode* root){
	if (root == NULL) return;
	freeHuffmanTree(root->left);
	freeHuffmanTree(root->right);
	free(root);
}
