#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char c[128]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int fd, sz;
    char *infile = argv[1];

    if(argc != 2){
        printf("Usage: %s filename\n", argv[0]);
        exit(-1);
    }
    
    //open input file
    printf("Opening %s ...\n", infile );
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror("open()"); exit(1); }

    printf("Reading 13 bytes from %s ...\n", infile );
    sz = read(fd, c, 13);
    printf("%d bytes were read: \"%s\".\n\n", sz, c);

    printf("Reading 99 more bytes from %s ...\n", infile );
    sz = read(fd, c, 99);
    printf("%d bytes were read: \"%s\".\n", sz, c);

    close(fd);
    exit(0);
}
