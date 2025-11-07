#include <stdio.h>
#include <string.h>
#include "huffman_codec.h"

//static since only used in this file
static void printUsage(const char* programName) {
	printf("Usage: %s [-z | -u] <input_file> <output_file>\n", programName);
	printf("  -z    Compress (zip) the input file\n");
	printf("  -u    Decompress (unzip) the input file\n");
	printf("\nExamples:\n");
	printf("  %s -z input.txt compressed.huf\n", programName);
	printf("  %s -u compressed.huf output.txt\n", programName);
}

int main(int argc, char *argv[]){

	if (argc != 4) {
		printUsage(argv[0]);
		return 1;
	}

	const char* mode = argv[1];
	const char* inputFile = argv[2];
	const char* outputFile = argv[3];

	if (strcmp(mode, "-z") == 0) {
		printf("Zipping %s -> %s\n", inputFile, outputFile);
		return compressFile(inputFile, outputFile);
	} else if (strcmp(mode, "-u") == 0) {
		printf("Unzipping %s -> %s\n", inputFile, outputFile);
		return decompressFile(inputFile, outputFile);
	} else {
		printf("Error: Invalid mode '%s'\n\n", mode);
		printUsage(argv[0]);
		return 1;
	}

        return 0;
}
