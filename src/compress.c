#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../include/huffman_codec.h"
#include "../include/LZ77.h"

static void printUsage(const char* programName) {
	printf("Usage: %s [options] <input file> <output file>\n", programName);
	printf("\nCompression Options:\n");
	printf("  -zh    Compress using Huffman coding\n");
	printf("  -zl    Compress using LZ77\n");
	printf("\nDecompression Options:\n");
	printf("  -uh    Decompress using Huffman coding\n");
	printf("  -ul    Decompress using LZ77\n");
}

int main(int argc, char *argv[]){
	if (argc != 4) {
		printUsage(argv[0]);
		return 1;
	}

	const char* mode = argv[1];
	const char* inputFile = argv[2];
	const char* outputFile = argv[3];

	if (strcmp(mode, "-zh") == 0){
		printf("Compressing with Huffman: %s -> %s\n", inputFile, outputFile);
		return compressFile(inputFile, outputFile);
	} else if (strcmp(mode, "-uh") == 0) {
		printf("Decompressing Huffman: %s -> %s", inputFile, outputFile);
		return decompressFile(inputFile, outputFile);
	} else if (strcmp(mode, "-zl" )== 0) {
		printf("Compressing with LZ77: %s -> %s\n", inputFile, outputFile);

		FILE *in = fopen(inputFile, "rb");
		if (!in) {
			printf("Error: cannot open input file '%s'\n", inputFile);
			return 1;
		}

		FILE *out = fopen(outputFile, "wb");
		if (!out) {
			printf("Error: cannot create output file '%s'\n", outputFile);
			fclose(in);
			return 1;
		}

		compressLZ77(in, out);

		fclose(in);
		fclose(out);

		printf("LZ77 compression complete\n");
		return 0;
	} else if (strcmp(mode, "-ul") == 0){
		printf("Decompressing LZ77: %s -> %s\n", inputFile, outputFile);

		FILE *in = fopen(inputFile, "rb");
		if (!in) {
			printf("Error: cannot open input file '%s'\n", inputFile);
			return 1;
		}

		FILE *out = fopen(outputFile, "wb");
		if (!out) {
			printf("Error: cannot create output file '%s'\n", outputFile);
			fclose(in);
			return 1;
		}

		decompressLZ77(in, out);

		fclose(in);
		fclose(out);

		printf("LZ77 decompression complete\n");
		return 0;
	} else {
		printf("Error: Invalid mode '%s'\n\n", mode);
		printUsage(argv[0]);
		return 1;
	}

	return 0;
}
