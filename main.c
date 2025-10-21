#include <stdio.h>
#include <stdlib.h> // file io
#include <string.h> // strcmp

#define NUMARGS 3

int main(int argc, char *argv[]){

        char usage[] = "\
usage: zipper [ -z / -u ] [input] [output] \n\
        file compression algorithm \n\
        written in C \n\
        Aden Hilderbrand\n";
        if (argc < NUMARGS) {
                printf("%s", usage);
                return 0;
        }
        if (strcmp(argv[1], "-z") == 0) {
                printf("Compress %s -> %s ?\n>> ", argv[2], argv[3]);
                //if yes
                //      compress_file()
                //if no
                //      exit 
        } else if (strcmp(argv[1], "-u") == 0) {
                printf("Unzip %s -> %s ?\n>> ", argv[2], argv[3]);
                //if yes
                //     decompress_file ()
                //if no
                //      exit 
        } else {
                printf("%s", usage);
                return 0;
        }
        return 0;
}
