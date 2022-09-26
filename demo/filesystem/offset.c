#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[]){

    char buf[100];
    int fd;
    int amt, pos;

    fd = open("in1.txt", O_RDONLY);
    if (fd < 0){
        perror("open()");
    }

    amt = read(fd, buf, 10);
    printf("The amount read was %d\n", amt);
    printf("Text read: %s\n\n", buf);

    pos = lseek(fd, 0, SEEK_CUR);
    printf("The value of pos is %d\n\n", pos);

    pos = lseek(fd, 0, SEEK_SET);
    printf("The value of pos is %d\n", pos);

    amt = read(fd, buf, 10);
    printf("The amount read was %d\n", amt);
    printf("Text read: %s\n\n", buf);

    pos = lseek(fd, -5, SEEK_END);
    printf("The value of pos is %d\n", pos);

    amt = read(fd, buf, 10);
    printf("The amount read was %d\n", amt);
    printf("Text read: %s\n\n", buf);

    int size = lseek(fd, 0, SEEK_END);
    printf("The size of the file is %d\n", size);

    // amt = read(fd, buf, 12);
    // printf("The amount read was %d\n", amt);
    // printf("Text read: %s\n\n", buf);

    // amt = read(fd, buf, 50);
    // printf("The amount read was %d\n", amt);
    // printf("Text read: %s\n", buf);

    return 0;
}