/* ls3.c
   James S. Plank
   CS360 -- Spring, 1994

   This is a program which lists files and their sizes to standard output.
   The files are identified by using opendir/readdir/closedir.

   Modified Oct 2021 by Steve La Fleur
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int get_size( const char * dirname ){
    struct stat buf;
    int exists, total_size=0;
    DIR *d;
    struct dirent *de;
    char * fullname;

    d = opendir(dirname);
    if (d == NULL) {
        fprintf(stderr, "Couldn't open \".\"\n");
        exit(1);
    }

    fullname = (char *)malloc(sizeof(char)*(strlen(dirname)+258));

    for (de = readdir(d); de != NULL; de = readdir(d)) {
        sprintf(fullname, "%s/%s", dirname, de->d_name);



        exists = lstat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found\n", fullname);
        }
        else if (S_ISDIR(buf.st_mode)) {
            printf("%-10lld %s/\n", (long long int)buf.st_size, fullname);
        } else if (S_ISLNK(buf.st_mode)) {
            printf("%-10lld %s@\n", (long long int)buf.st_size, fullname);
        } else if (buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
            printf("%-10lld %s*\n", (long long int)buf.st_size, fullname);
        } else {
            printf("%-10lld %s\n", (long long int)buf.st_size, fullname );
        }
        total_size+=buf.st_size;
        
        /* Make the recursive call if the file is a directory */
        if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            total_size += get_size(fullname);
        }
    }
    
    closedir(d);
    return total_size;
}

int main(int argc, char **argv)
{

    printf("%10d total\n", get_size(".") );
    
    return 0;
}
