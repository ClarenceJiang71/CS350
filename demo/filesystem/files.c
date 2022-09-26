#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[]){
    char *outfilename1, *outfilename2;
    FILE *f;
    int fd;

    if (argc < 3){
        fprintf(stderr, "Usage: %s outfilename1 outfilename2\n", argv[0]);
        exit(-1);
    }

    outfilename1 = argv[1];
    outfilename2 = argv[2];
    //0x61 is hex ASCII encoding for 'a'
    //0x62 is hex ASCII encoding for 'b'
    //0x63 is hex ASCII encoding for 'c'
    //0x64 is hex ASCII encoding for 'd'
    int i = 0x60626364;
    const char *s = "Number: ";

    //compare convenience of fopen() to open()
    f = fopen(outfilename1, "w");
    if (f < 0) {
        perror(outfilename1);
        exit(-1);
    }

    fd = open(outfilename2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0){
        perror(outfilename2);
        exit(-1);
    }

    //compare convenience of fprintf() to write()
    // fprintf(f, "%s%d\n", s, i);
    fprintf(f, "%s%x\n", s, i);

    write(fd, s, strlen(s));
    write(fd, &i, sizeof(i));
    write(fd, "\n", 1);

    fclose(f);
    close(fd);

    return 0;
}
