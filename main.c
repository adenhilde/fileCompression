#include <stdio.h>

#define NUMARGS 3

char usage[] = "\
usage: zipper [ -z / -u ] [input] [output] \n\
        file compression algorithm \n\
        written in C \n\
        Aden Hilderbrand\n";

int main(int argc, char *argv[]){
        if (argc < NUMARGS) {
                printf("%s", usage);
                return 0;
        }

        return 0;
}
