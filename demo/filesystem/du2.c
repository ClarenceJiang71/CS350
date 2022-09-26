#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    struct stat buf;

    if(argc < 2){
        fprintf(stderr, "No file specified.");    
        exit(-1);
    }

    char *filename = argv[1];
    stat(filename, &buf);

    int size = buf.st_size;
    printf("%-10d %s\n", size, filename);    

}