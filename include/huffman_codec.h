#ifndef HUFFMAN_CODEC_H
#define HUFFMAN_CODEC_H

#include "huffman_tree.h"
#include<stddef.h>
#include<stdint.h>
//using stdint allows for consistent int sizes on the lower level no matter the OS. though this program is still best used on linux


typedef struct {
	uint32_t magic;
	uint32_t originalSize;
	uint32_t numChars;
	uint8_t paddingBits;
} FileHeader;

#define MAGIC_NUMBER 0x46465548
#define BUFFER_SIZE 8192

int compressFile(const char* inputFile, const char* outputFile);
int decompressFile(const char* inputFile, const char* outputFile);

#endif
