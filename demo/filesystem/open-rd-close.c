#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[] )
{
    int fd1, fd2;
    char * infile = argv[1];

    if(argc != 2){
        printf("Usage: %s filename\n", argv[0]);
        exit(1);
    }
    
    //open input file
    fd1 = open(infile, O_RDONLY);
    if( fd1 < 0 ) {
        perror("open()");
    }
    else{
        printf("open() returned fd1: %d\n", fd1);
    }

    // close input file
    if ( close(fd1) < 0 ) {
        perror("close()");
    }
    else{
        printf("close(%d)\n", fd1 );
    }
    return 0;
}
