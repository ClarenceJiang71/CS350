#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int fd, size;


    if(argc < 2)
        fprintf(stderr, "No file specified.");
    
    char *filename = argv[1];
    fd = open(filename, O_RDONLY);
    if(fd < 0){
        perror("open()");
        exit(-1);
    }
    
    size = lseek(fd, (off_t)0, SEEK_END);

    printf("%-10d %s\n", size, filename);




    return 0;
}