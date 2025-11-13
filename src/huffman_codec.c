#include "../include/huffman_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void storeCodes(MinHeapNode *root, int arr[], int top, char codes[256][MAX_TREE_HEIGHT]) {
	if (root->left) {
		arr[top] = 0;
		storeCodes(root->left, arr, top+1, codes);
	}

	if (root->right) {
		arr[top] = 1;
		storeCodes(root->right, arr, top+1, codes);
	}

	if (!(root->left) && !(root->right)) {
		for(int i=0; i < top; i++){
			codes[(unsigned char)root->data][i] = arr[i] + '0';
		}
		codes[(unsigned char)root->data][top] = '\0';
	}
}

static void generateHuffmanCodes(unsigned char data[], int freq[], int size, char codes[256][MAX_TREE_HEIGHT]) {
	MinHeapNode* root = buildHuffmanTree(data, freq, size);
	int arr[MAX_TREE_HEIGHT], top = 0;

	for (int i = 0; i < 256; i++) {
		codes[i][0] = '\0';
	}

	storeCodes(root, arr, top, codes);
	freeHuffmanTree(root);
}

static int countFrequencies(const char* filename, int freq[256], size_t* fileSize) {
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		printf("Error: cannot open file '%s'\n", filename);
		return 0;
	}

	unsigned char buffer[BUFFER_SIZE];
	size_t bytesRead;
	size_t totalBytes = 0;

	for (int i = 0; i < 256; i++){
		freq[i] = 0;
	}

	while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
		for (size_t i = 0; i < bytesRead; i++){
			freq[buffer[i]]++;
		}
		totalBytes += bytesRead;
	}

	fclose(fp);

	if (totalBytes == 0) {
		printf("Error: file is empty\n");
		return 0;
	}

	*fileSize = totalBytes;
	return 1;
}

typedef struct {
	uint8_t* buffer;
	size_t bufferSize;
	size_t position;
	uint8_t currentByte;
	int bitCount;
	size_t totalBits;
	FILE* output;
} BitWriter;

static void initBitWriter(BitWriter* writer, FILE* output){
	writer->bufferSize = BUFFER_SIZE;
	writer->buffer = (uint8_t*)malloc(writer->bufferSize);
	writer->position = 0;
	writer->currentByte = 0;
	writer->bitCount = 0;
	writer->totalBits = 0;
	writer->output = output;
}

static int flushBitWriter(BitWriter* writer) {
	if (writer->position > 0) {
		size_t written = fwrite(writer->buffer, 1, writer->position, writer->output);
		if (written != writer->position) {
			printf("Error: could not write to output file\n");
			return 0;
		}
		writer->position = 0;
	}
	return 1;
}

static int writeBit(BitWriter* writer, int bit) {
	writer->currentByte = (writer->currentByte << 1) | bit;
	writer->bitCount++;
	writer->totalBits++;

	if (writer->bitCount == 8) {
		writer->buffer[writer->position++] = writer->currentByte;

		if (writer->position == writer->bufferSize) {
			if (!flushBitWriter(writer)){
				return 0;
			}
		}

		writer->currentByte = 0;
		writer->bitCount = 0;
	}

	return 1;
}

static int finalizeBitWriter(BitWriter* writer){
	if (writer->bitCount > 0) {
		writer->currentByte <<= (8 - writer->bitCount);
		writer->buffer[writer->position++] = writer->currentByte;
	}

	if (!flushBitWriter(writer)){
		return 0;
	}

	free(writer->buffer);
	return 1;
}

static int encodeFile(const char* filename, char codes[256][MAX_TREE_HEIGHT], FILE* output, size_t* totalBits){
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		printf("Error: cannot open file '%s' for encoding\n", filename);
		return 0;
	}

	BitWriter writer;
	initBitWriter(&writer, output);

	unsigned char buffer[BUFFER_SIZE];
	size_t bytesRead;

	while((bytesRead = fread(buffer, 1, BUFFER_SIZE, fp)) > 0){
		for (size_t i = 0; i < bytesRead; i++){
			char* code = codes[buffer[i]];

			for(int j = 0; code[j] != '\0'; j++){
				if(!writeBit(&writer, code[j] - '0')){
					fclose(fp);
					free(writer.buffer);
					return 0;
				}
			}
		}
	}

	*totalBits = writer.totalBits;

	if(!finalizeBitWriter(&writer)){
		fclose(fp);
		return 0;
	}

	fclose(fp);
	return 1;
}

int compressFile(const char* inputFile, const char* outputFile){
	printf("Compressing '%s' -> '%s'...\n", inputFile, outputFile);

	int freq[256];
	size_t fileSize;

	if(!countFrequencies(inputFile, freq, &fileSize)){
		return 1;
	}

	printf("Read %zu bytes from file\n", fileSize);

	unsigned char data[256];
	int freqArray[256];
	int numChars = 0;

	for (int i = 0; i < 256; i++){
		if(freq[i] > 0){
			data[numChars] = (unsigned char)i;
			freqArray[numChars] = freq[i];
			numChars++;
		}
	}

	if (numChars == 0){
		printf("Error: no data found in file\n");
		return 1;
	}

	printf("Found %d unique bytes\n", numChars);

	char codes[256][MAX_TREE_HEIGHT];
	generateHuffmanCodes(data, freqArray, numChars, codes);

	FILE* outFile = fopen(outputFile, "wb");
	if (!outFile){
		printf("Error: cannot create output file '%s'\n", outputFile);
		return 1;
	}

	FileHeader header;
	header.magic = MAGIC_NUMBER;
	header.originalSize = (uint32_t)fileSize;
	header.numChars = (uint32_t)numChars;
	header.paddingBits = 0;

	if (fwrite(&header, sizeof(FileHeader), 1, outFile) != 1){
		printf("Error: failed to write header");
		fclose(outFile);
		return 1;
	}

	for (int i = 0; i < numChars; i++){
		if(fwrite(&data[i], sizeof(unsigned char), 1, outFile) != 1){
			printf("Error: cannot write frequency table");
			fclose(outFile);
			return 1;
		}
		uint32_t f = (uint32_t)freqArray[i];
		if(fwrite(&f, sizeof(uint32_t), 1, outFile) != 1){
			printf("Error: cannot write frequency table\n");
			fclose(outFile);
			return 1;
		}
	}

	size_t totalBits;
	if (!encodeFile(inputFile, codes, outFile, &totalBits)){
		fclose(outFile);
		return 1;
	}

	uint8_t paddingBits = (8 - (totalBits % 8)) % 8;
	header.paddingBits = paddingBits;

	if (fseek(outFile, 0, SEEK_SET) != 0){
		printf("Error: cannot seek to update header\n");
		fclose(outFile);
		return 1;
	}

	if(fwrite(&header, sizeof(FileHeader), 1, outFile) != 1){
		printf("Error: cannot update header\n");
		fclose(outFile);
		return 1;
	}

	fseek(outFile, 0, SEEK_END);
	long compressedSize = ftell(outFile);
	fclose(outFile);

	printf("\nCompression Stats\n");
	printf("Original Size:       %zu bytes (%zu bits)\n", fileSize, fileSize*8);
	printf("Compressed size:     %ld bytes (%zu bits)\n", compressedSize, totalBits);
	printf("Compression ratio:   %.2f%%\n", (1.0 - (double)compressedSize / fileSize) * 100);
	printf("Space saved:         %zu bytes\n", fileSize - compressedSize);
	printf("\nCompression complete\n");

	return 0;
}

int decompressFile(const char* inputFile, const char* outputFile){
	printf("Decompressing '%s' -> '%s'...\n", inputFile, outputFile);

	FILE* inFile = fopen(inputFile, "rb");
	if (!inFile){
		printf("Error: cannot open input file '%s'\n", inputFile);
		return 1;
	}

	FileHeader header;
	if (fread(&header, sizeof(FileHeader), 1,inFile) != 1){
		printf("Error: cannot read header\n");
		return 1;
	}

	if (header.magic != MAGIC_NUMBER){
		printf("Error: invalid file format");
		fclose(inFile);
		return 1;
	}

	unsigned char chars[256];
	int freqs[256];

	for (uint32_t i = 0; i < header.numChars; i++){
		if(fread(&chars[i], sizeof(unsigned char), 1, inFile) != 1){
			printf("Error: cannot read frequency table");
			fclose(inFile);
			return 1;
		}
		uint32_t f;
		if (fread(&f, sizeof(uint32_t), 1, inFile) != 1){
			printf("Error: cannot read frequency table\n");
			fclose(inFile);
			return 1;
		}
		freqs[i] = (int)f;
	}

	MinHeapNode* root = buildHuffmanTree(chars, freqs, header.numChars);

	FILE* outFile = fopen(outputFile, "wb");
	if (!outFile){
		printf("Error: cannot create output file '%s'\n", outputFile);
		freeHuffmanTree(root);
		fclose(inFile);
		return 1;
	}

	unsigned char inBuffer[BUFFER_SIZE];
	unsigned char outBuffer[BUFFER_SIZE];
	size_t outPos = 0;
	size_t bytesDecoded = 0;
	MinHeapNode* current = root;

	size_t bytesRead;
	while ((bytesRead = fread(inBuffer, 1, BUFFER_SIZE, inFile)) > 0 && bytesDecoded < header.originalSize) {
		for (size_t i = 0; i < bytesRead && bytesDecoded < header.originalSize; i++) {
			uint8_t byte = inBuffer[i];

			for (int bit = 7; bit >= 0 && bytesDecoded < header.originalSize; bit--) {
				int bitValue = (byte >> bit) & 1;

				if (bitValue == 0)
					current = current->left;
				else
					current = current->right;

				if (current->left == NULL && current->right == NULL) {
					// leaf = decoded symbol
					outBuffer[outPos++] = current->data;
					bytesDecoded++;

					if (outPos == BUFFER_SIZE) {
						fwrite(outBuffer, 1, outPos, outFile);
						outPos = 0;
					}

					current = root;
				}
			}
		}
	}


	if (outPos > 0 && fwrite(outBuffer, 1, outPos, outFile) != outPos){
		printf("Error: cannot write to output file '%s'\n", outputFile);		
		freeHuffmanTree(root);
		fclose(inFile);
		fclose(outFile);
		return 1;
	}

	printf("\nDecompression Stats:\n");
	printf("Decompressed size: %u bytes\n", header.originalSize);
	printf("\nDecompression complete\n");

	freeHuffmanTree(root);
	fclose(inFile);
	fclose(outFile);
	return 0;
}

