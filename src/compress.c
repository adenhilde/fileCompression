#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../include/huffman_codec.h"
#include "../include/LZ77.h"

static long getFileSize(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

static void printUsage(const char* programName) {
	printf("Usage: %s [options] <input file> <output file>\n", programName);
	printf("\nCompression Options:\n");
	printf("  -z    Compress using LZ77 + Huffman coding\n");
	printf("  -zh    Compress using Huffman coding\n");
	printf("  -zl    Compress using LZ77\n");
	printf("\nDecompression Options:\n");
	printf("  -u    Decompress using Huffman then LZ77\n");
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

	if (strcmp(mode, "-z") == 0){
		printf("Compressing with LZ77+Huffman: %s -> %s\n", inputFile, outputFile);
		const char* tempFile = "temp_lz77.bin";

		long originalSize = getFileSize(inputFile);

		FILE *in = fopen(inputFile, "rb");
		if (!in){
			printf("Error: cannot open input file '%s'\n", inputFile);
			return 1;
		}
		FILE *temp = fopen(tempFile, "wb");
		if (!temp){
			printf("Error: cannot create temporary file\n");
			fclose(in);
			return 1;
		}

		printf("Compressing with LZ77...\n");
		compressLZ77(in, temp);
		fclose(in);
		fclose(temp);

		printf("Compressing with Huffman...\n");
		int result = compressFile(tempFile, outputFile);

		// deletes the temp file
		remove(tempFile);

		if (result == 0){
			long compressedSize = getFileSize(outputFile);
			printf("\nCompression Stats\n");
			printf("Original Size:       %ld bytes\n", originalSize);
			printf("Compressed size:     %ld bytes\n", compressedSize);
			printf("Compression ratio:   %.2f%%\n", (1.0 - (double)compressedSize / originalSize) * 100);
			printf("Space saved:         %ld bytes\n", originalSize - compressedSize);
			printf("\nCompression Complete\n");
		}
		return result;

	} else if (strcmp(mode, "-zh") == 0){

		long originalSize = getFileSize(inputFile);

		printf("Compressing with Huffman: %s -> %s\n", inputFile, outputFile);
		int result = compressFile(inputFile, outputFile);

		if (result == 0){
			long compressedSize = getFileSize(outputFile);
			printf("\nHuffman Compression Stats\n");
			printf("Original Size:       %ld bytes\n", originalSize);
			printf("Compressed size:     %ld bytes\n", compressedSize);
			printf("Compression ratio:   %.2f%%\n", (1.0 - (double)compressedSize / originalSize) * 100);
			printf("Space saved:         %ld bytes\n", originalSize - compressedSize);
			printf("\nCompression Complete\n");
		}
		return result;

	}else if (strcmp(mode, "-u") == 0){

		printf("Decompressing with Huffman + LZ77: %s -> %s\n", inputFile, outputFile);
		const char* tempFile = "temp_huff.bin";

		printf("Decompressing Huffman Coding...\n");
		int result = decompressFile(inputFile, tempFile);
		if (result != 0) {
			return result;
		}

		printf("Decompressing LZ77...\n");
		FILE *temp = fopen(tempFile, "rb");
		if (!temp){
			printf("Error: cannot open temporary file\n");
			return 1;
		}

		FILE *out = fopen(outputFile, "wb");
		if (!out) {
			printf("Error: cannot create output file '%s'\n", outputFile);
			fclose(temp);
			return 1;
		}

		decompressLZ77(temp, out);
		fclose(temp);
		fclose(out);

		// deletes the temp file
		remove(tempFile);
		
		printf("\nDecompression complete\n");
		return 0;

	} else if (strcmp(mode, "-uh") == 0) {

		printf("Decompressing Huffman: %s -> %s\n", inputFile, outputFile);
		return decompressFile(inputFile, outputFile);

	} else if (strcmp(mode, "-zl" )== 0) {
		printf("Compressing with LZ77: %s -> %s\n", inputFile, outputFile);

		long originalSize = getFileSize(inputFile);

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

		long compressedSize = getFileSize(outputFile);
		printf("\nLZ77 Compression Stats\n");
		printf("Original Size:       %ld bytes\n", originalSize);
		printf("Compressed size:     %ld bytes\n", compressedSize);
		printf("Compression ratio:   %.2f%%\n", (1.0 - (double)compressedSize / originalSize) * 100);
		printf("Space saved:         %ld bytes\n", originalSize - compressedSize);
		printf("\nLZ77 compression complete\n");
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
