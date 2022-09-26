#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[]){
    DIR *dir;
    struct dirent *entry;
    struct stat buf;

    char filename[2] = ".";
    dir = opendir(filename);
    if (dir == NULL){
        perror("opendir()");
        exit(-1);
    }

    for(entry = readdir(dir); entry != NULL; entry = readdir(dir)){
        char *entname = entry->d_name;
        stat(entname, &buf);
        int size = buf.st_size;
        printf("%-10d %s\n", size, entname);  
    }

    closedir(dir);

    return 0;
}