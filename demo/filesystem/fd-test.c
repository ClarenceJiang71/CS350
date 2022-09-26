#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>


int main(int argc, char *argv[]){
    close(1);

    int fd = open("othernewfile.txt", O_WRONLY|O_CREAT, 0644);
    if (fd == -1)
        perror("Big problem");
    printf("Hello, this is a test");
    return 0;
}