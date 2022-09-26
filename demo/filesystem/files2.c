#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[]){
    char *outfilename1;
    int fd;

    if (argc < 2){
        fprintf(stderr, "Usage: %s outfilename1\n", argv[0]);
        exit(-1);
    }

    outfilename1 = argv[1];
    char c = 0;

    fd = open(outfilename1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror(outfilename1);
        exit(-1);
    }

    write(fd, "NULL: ", 6);
    write(fd, &c, sizeof(char));
    write(fd, "\n", 1);
    return 0;
}