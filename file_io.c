#include <stdio.h>
#include <stdlib.h>

char * readFile(const char* filename, size_t* fileSize) {
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		printf("Error: Cannot open file '%s'\n", filename);
		return NULL;
	}
	
	if (fseek(fp, 0, SEEK_END) != 0){
		printf("Error: Cannot seek to end of file\n");
		fclose(fp);
		return NULL;
	}

	long fsize = ftell(fp);
	if (fsize < 0) {
		printf("Error: Cannot determine file size of '%s'\n", filename);
		fclose(fp);
		return NULL;
	}

	if (fsize == 0){
		printf("Error: File '%s' is empty\n", filename);
	}

	if (fseek(fp, 0, SEEK_SET) != 0) {
		printf("Error: Cannot seek to beginning of file '%s'\n", filename);
		return NULL;
	}
	// more error handling stuff to be done
	// max file size issues?
	
	//actually reading the file now
	char* buffer = (char*)malloc(fsize+1);
	if (!buffer) {
		printf("Error: Cannot allocate memory for file (%ld bytes)\n", fsize+1);
		fclose(fp);
		return NULL;
	}

	size_t bytesRead = fread(buffer, 1, fsize, fp);
	if (bytesRead != (size_t)fsize) {
		printf("Error: Could not read entire file\nRead %zu of %ld bytes\n", bytesRead, fsize);
		free(buffer);
		fclose(fp);
		return NULL;
	}

	// add terminating character to the end of the buffer
	buffer[fsize] = '\0';
	fclose(fp);

	if (fileSize) {
		*fileSize = (size_t)fsize;
	}

	return buffer;
}

int writeFile(const char* filename, const void* data, size_t size) {
	FILE* fp = fopen(filename, "wb");
	if (!fp) {
		printf("Error: Cannot create file '%s'\n", filename);
		return 0;
	}

	// using fwrite instead of fprintf 
	// allows writing in hex for a smaller size
	size_t written = fwrite(data, 1, size, fp);
	if (written != size) {
		printf("Error: Could not write entire file!\nWrote %zu of %zu bytes\n", written, size);
		fclose(fp);
		return 0;
	}

	fclose(fp);
	return 1;
}
