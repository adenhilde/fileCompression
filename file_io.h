#ifndef FILE_IO_H
#define FILE_IO_H

#include <stddef.h>

//file io functions
char* readFile(const char* filename, size_t* fileSize);
int writeFile(const char* filename, const void* data, size_t size);

#endif

