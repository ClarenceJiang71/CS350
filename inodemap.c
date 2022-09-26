//
//  inodemap.c
//  mytar
//
//  Created by Dorian Arnold on 10/8/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//

#include "inodemap.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

char const ** Map=NULL;

void set_inode( ino_t i, const char * f ){
    uint32_t mappos = i % MAPSIZE;

    if( Map == NULL )
        Map = (const char  **)calloc(MAPSIZE, sizeof(char*));

    Map[mappos] = f;
}

const char * get_inode( ino_t i ){
    if( Map == NULL )
        Map = (const char  **)calloc(MAPSIZE, sizeof(char*));

    return Map[ i % MAPSIZE ];
}

int magicNumber = 0x7261746D;

void create(char * dirname, char * infile);
void add_content( FILE * output, char * dirname, int index);
void print( char * infile);
void extract( char * infile);

int main( int argc, char *argv[] )
{
    char * infile;
    char * result_name;
    int o;
    char opt = 'n'; //set the default case as transfer the input file to PBM
    int opt_count = 0; // count the number of operation
    int have_output = 0; // check if there is a output file


    extern char *optarg;
    extern int optind;

    while ((o = getopt(argc, argv, "cxtf:")) != -1) {
        switch (o) {
        case 'c':
            // if the command includes two or more operation on the infile, stop the execution 
            if(opt_count>0){
                fprintf(stderr, "Error: Multiple modes specified.\n");
            }
            opt = 'c';
            // mark we have an operation
            opt_count++;
            break;

        case 'x':
            if(opt_count>0){
                fprintf(stderr, "Error: Multiple modes specified.\n");
            }
            opt = 'x';
            opt_count++;
            break;

        case 't':
            if(opt_count>0){
                fprintf(stderr, "Error: Multiple modes specified.\n");
            }
            opt = 't';
            opt_count++;
            break;

        case 'f':
            //if there is no operation
            if(opt_count==0){
                fprintf(stderr, "Error: No mode specified.\n");
            }
            // if there is already a '-f' command
            if(have_output>0){
                fprintf(stderr, "Error: Multiple transformations specified\n");
            }
            have_output ++;
            result_name = optarg;
            break;

        default: 
            //if the operation is not recognized
            fprintf(stderr, "Error: No mode specified.\n");
            break;
        }
    }

    // get the infile
    infile = argv[optind];

    // execute the operations
    switch (opt) {
        case 'c':
            create(infile,result_name);
            break;
        case 'x':
            extract(result_name);
            break;
        case 't':
            print(result_name);
            break;

        default: 
            fprintf(stderr, "Error: No tarfile specified\n");
            break;
    }
    return 0;
}




// Helper Functions.
void create(char * dirname, char * infile){
    // To Write the stat of the target tar file
    struct stat buf0;
    int exists0, index, i, length;
    FILE *output;
    DIR *d;
    char * fullname;
    char dir[strlen(dirname)+1];

    d = opendir(dirname);
    if (d == NULL) {
        fprintf(stderr, "Specified target %s is not a directory. \n", dirname);
        exit(-1);
    }

    // Create and open the target tar file.
    output = fopen(infile, "w+");
    
    length = strlen(dirname);
    //copy the directory
    strncpy(dir,dirname,length);
    //check if the last char is '/', if yes get rid of it
    if(dir[strlen(dirname)-1]=='/'){
        length = length - 1;
    }
    //have the directory name after eliminate the '/'
    char true_dir[length+1];
    strncpy(true_dir,dirname,length);
    true_dir[length] = '\0';
    // find the last index of '/'
    index = 0;
    for(i=0;i<length;i++){
        if(true_dir[i]=='/'){
            index = i;
        }
    }

    fullname = (char *)malloc(sizeof(char)*(strlen(dirname)+258));
    //find the dirname after eliminating the path
    if(index == 0){
        char true_dirname[length+1];
        strncpy(true_dirname, true_dir, length+1);
        sprintf(fullname, "%s", true_dirname);
    }
    else{
        char true_dirname[length-index];
        strncpy(true_dirname, true_dir+index+1, length-index);
        sprintf(fullname, "%s", true_dirname);
    }
    // Write the magic number first! Tarsonal identification.
    int check0 = fwrite(&magicNumber, 1, sizeof(int), output);
    if (check0 < sizeof(int)) {
        perror("fwrite");
        exit(-1);
    }
    // printf("fullname:%s\n",fullname);
    exists0 = lstat(fullname, &buf0);
    if (exists0 < 0) {
        perror("lstat");
        exit(-1);
    }
    long long int inode = (long long int)buf0.st_ino;
    int name_length = (int) strlen(fullname);
    int mode = (int) buf0.st_mode;
    long long int time = (long long int)buf0.st_mtime;
    long file_size = (long)buf0.st_size;

    if( !get_inode( buf0.st_ino ) ) {
        set_inode(buf0.st_ino, fullname);
        if (S_ISDIR(buf0.st_mode)) {
            // Write file inode number
            check0 = fwrite(&inode, 1, sizeof(long long int), output);
            if (check0 < sizeof(long long int)) {
                perror("fwrite");
                exit(-1);
            }
            // Write file name length
            check0 = fwrite(&name_length, 1, sizeof(int), output);
            if (check0 < sizeof(int)) {
                perror("fwrite");
                exit(-1);
            }
            // Write file name
            check0 = fwrite(fullname, 1, name_length, output);
            if (check0 < name_length) {
                perror("fwrite");
                exit(-1);
            }
            // Write file mode
            check0 = fwrite(&mode, 1, sizeof(int), output);
            if (check0 < sizeof(int)) {
                perror("fwrite");
                exit(-1);
            }
            // Write file modification time
            check0 = fwrite(&time, 1, sizeof(long long int), output);
            if (check0 < sizeof(long long int)) {
                perror("fwrite");
                exit(-1);
            }
        }
    }
    add_content(output, true_dir, index);
    closedir(d);
    fclose(output);
}

void add_content( FILE * output, char * dirname, int index) {
    DIR *d;
    struct dirent *de;
    struct stat buf;
    int exists, exist;
    char * fullname, tmp, true_dirname;
    char * fullname_path;
    int check;
    int length = strlen(dirname);

    d = opendir(dirname);
    if (d == NULL) {
        fprintf(stderr, "Specified target %s is not a directory. \n", dirname);
        exit(-1);
    }

    fullname = (char *)malloc(sizeof(char)*(strlen(dirname)+258));
    fullname_path = (char *)malloc(sizeof(char)*(strlen(dirname)+258));

    for (de = readdir(d); de != NULL; de = readdir(d)) {
        if(index == 0){
            char true_dirname[length];
            strncpy(true_dirname, dirname, length);
            sprintf(fullname, "%s/%s", true_dirname, de->d_name);
        }
        else{
            char true_dirname[length-index];
            strncpy(true_dirname, dirname+index+1, length-index);
            sprintf(fullname, "%s/%s", true_dirname, de->d_name);
        }

        exists = lstat(fullname, &buf);
        if (exists < 0) {
            perror("lstat");
            exit(-1);
        }

        long long int inode = (long long int)buf.st_ino;
        int name_length = (int) strlen(fullname);
        int mode = (int) buf.st_mode;
        long long int time = (long long int)buf.st_mtime;
        long file_size = (long)buf.st_size;

        // Ignore ., .., mytar, and the target tar file
        if (strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0) {
            if( !get_inode( buf.st_ino )) {
                //inode not yet seen; add to list and process
                set_inode( buf.st_ino, fullname );
                // Directories
                if (S_ISDIR(buf.st_mode)) {
                    // Write file inode number
                    check = fwrite(&inode, 1, sizeof(long long int), output);
                    if (check < sizeof(long long int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file name length
                    check = fwrite(&name_length, 1, sizeof(int), output);
                    if (check < sizeof(int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file name
                    check = fwrite(fullname, 1, name_length, output);
                    if (check < name_length) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file mode
                    check = fwrite(&mode, 1, sizeof(int), output);
                    if (check < sizeof(int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file modification time
                    check = fwrite(&time, 1, sizeof(long long int), output);
                    if (check < sizeof(long long int)) {
                        perror("fwrite");
                        exit(-1);
                    }

                }
                else if (S_ISLNK(buf.st_mode)) {
                }
                else {
                    // Write file inode number
                    check = fwrite(&inode, 1, sizeof(long long int), output);
                    if (check < sizeof(long long int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file name length
                    check = fwrite(&name_length, 1, sizeof(int), output);
                    if (check < sizeof(int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file name
                    check = fwrite(fullname, 1, name_length, output);
                    if (check < name_length) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file mode
                    check = fwrite(&mode, 1, sizeof(int), output);
                    if (check < sizeof(int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file modification time
                    check = fwrite(&time, 1, sizeof(long long int), output);
                    if (check < sizeof(long long int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    // Write file size
                    check = fwrite(&file_size, 1, sizeof(long long int), output);
                    if (check < sizeof(long long int)) {
                        perror("fwrite");
                        exit(-1);
                    }
                    //  Write file content
                    FILE *tmp = fopen(fullname, "r");
                    if (tmp == NULL) {
                        perror("fopen");
                        exit(-1);
                    }
 
                    char *buff = (char *)malloc(sizeof(char)*(file_size+1));
                    fread(buff, 1, file_size, tmp);
                    buff[file_size] = '\0';
                    check = fwrite(buff, 1, file_size, output);
                    if (check < strlen(buff)) {
                        perror("fwrite");
                        exit(-1);
                    }

                    free(buff);
                    fclose(tmp);
                }
            }
         
            else {
                // Write file inode number
                check = fwrite(&inode, 1, sizeof(long long int), output);
                if (check < sizeof(long long int)) {
                    perror("fwrite");
                    exit(-1);
                }
                // Write file name length
                check = fwrite(&name_length, 1, sizeof(int), output);
                if (check < sizeof(int)) {
                    perror("fwrite");
                    exit(-1);
                }
                // Write file name
                check = fwrite(fullname, 1, name_length, output);
                if (check < name_length) {
                    perror("fwrite");
                    exit(-1);
                }
            }

            /* Make the recursive call if the file is a directory */
            if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
                sprintf(fullname_path, "%s/%s", dirname, de->d_name);
                add_content(output, fullname_path, index);
            }
        }
    }
    closedir(d);
}

void print( char * infilename ) {
    FILE *infile;
    struct stat buf1;
    int exists1, check1;

    long long int inode, time, file_size;
    int magic, name_length, mode;
    char * fullname = (char *)malloc(sizeof(char)*999999);
    int read_size = 0;

    // Open the target tar file for reading.
    infile = fopen(infilename, "r");
    exists1 = lstat(infilename, &buf1);
    if (exists1 < 0) {
        perror("lstat");
        exit(-1);
    }
    // Check if the specified target is a tar file
    check1 = fread(&magic, 1, sizeof(int), infile);
    if(check1 < sizeof(int)) {
        perror("fread");
        exit(-1);
    }
    
    if (magic != 0x7261746D) {
        fprintf(stderr, "Bad magic number\n");
        exit(-1);
    }
    read_size += sizeof(int);

    while(read_size<buf1.st_size) {
        // Get the inode number
        check1 = fread(&inode, 1, sizeof(long long int), infile);
        if(check1 < sizeof(long long int)) {
            perror("fread");
            exit(-1);
        }
        read_size += sizeof(long long int);

        if( !get_inode((ino_t)inode)) {
            // Get the file name length
            check1 = fread(&name_length, 1, sizeof(int), infile);
            if(check1 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);
            // Get the file name
            char * name = (char *)malloc(sizeof(char)*(name_length+1));
            check1 = fread(name, 1, name_length, infile);
            if(check1 < name_length) {
                perror("fread");
                exit(-1);
            }
            name[name_length] = '\0';
            set_inode( (ino_t)inode,name);
            read_size += name_length;
            // Get the file mode
            check1 = fread(&mode, 1, sizeof(int), infile);
            if(check1 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);

            // Directories
            if(S_ISDIR((mode_t)mode)) {
                // Get the file modification time
                check1 = fread(&time, 1, sizeof(long long int), infile);
                if(check1 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);
                printf("%s/ inode: %llu, mode: %o, mtime: %llu\n", name, inode, mode & 0x1ff, time);
            } 
            else if(S_ISLNK((mode_t)mode)) {
                // Do Nothing
                // Executable files
            } 
            else if ((mode_t)mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                // Get the file modification time
                check1 = fread(&time, 1, sizeof(long long int), infile);
                if(check1 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);
                // Get the file size
                check1 = fread(&file_size, 1, sizeof(long long int), infile);
                if(check1 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);

                check1 = fseek(infile, file_size, SEEK_CUR);
                if (check1 != 0) {
                    perror("fseek");
                    exit(-1);
                }
                read_size += sizeof(file_size);
                printf("%s* inode: %llu, mode: %o, mtime: %llu, size: %llu\n", name, inode, mode & 0x1ff, time, file_size);
                // Regular files
            } 
            else {
                // Get the file modification time
                check1 = fread(&time, 1, sizeof(long long int), infile);
                if(check1 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);
                // Get the file size
                check1 = fread(&file_size, 1, sizeof(long long int), infile);
                if(check1 < sizeof(int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);

                check1 = fseek(infile, file_size, SEEK_CUR);
                if (check1 != 0) {
                    perror("fseek");
                    exit(-1);
                }
                read_size += file_size;

                printf("%s inode: %llu, mode: %o, mtime: %llu, size: %llu\n", name, inode, mode & 0x1ff, time, file_size);
            }
        } 
        else {
            // Get the file name length
            check1 = fread(&name_length, 1, sizeof(int), infile);
            if(check1 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);
            // Get the file name
            char * name = (char *)malloc(sizeof(char)*(name_length+1));
            check1 = fread(name, 1, name_length, infile);
            if(check1 < name_length) {
                perror("fread");
                exit(-1);
            }
            name[name_length] = '\0';
            read_size += name_length;

            printf("%s/ inode: %llu\n", name, inode);
        }
    }
    free(fullname);
    fclose(infile);
}

void extract( char * infilename ) {
    FILE *infile;
    struct stat buf2;
    int exists2, check2;

    long long int inode, time, file_size;
    int magic, name_length, mode;
    char * fullname = (char *)malloc(sizeof(char)*999999);
    int read_size = 0;
    int first = 0;

    // Open the target tar file for reading.
    infile = fopen(infilename, "r");
    exists2 = lstat(infilename, &buf2);
    if (exists2 < 0) {
        perror("lstat");
        exit(-1);
    }
    // Check if the specified target is a tar file
    check2 = fread(&magic, 1, sizeof(int), infile);
    if(check2 < sizeof(int)) {
        perror("fread");
        exit(-1);
    }
    
    if (magic != 0x7261746D) {
        fprintf(stderr, "Bad magic number\n");
        exit(-1);
    }
    read_size += sizeof(int);

    while(read_size<buf2.st_size) {
        // Get the inode number
        check2 = fread(&inode, 1, sizeof(long long int), infile);
        if(check2 < sizeof(long long int)) {
            printf("here\n");
            perror("fread");
            exit(-1);
        }
        read_size += sizeof(long long int);

        if( !get_inode((ino_t)inode) ) {
            // Get the file name length
            check2 = fread(&name_length, 1, sizeof(int), infile);
            if(check2 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);

            // Get the file name
            char * name = (char *)malloc(sizeof(char)*(name_length+1));
            check2 = fread(name, 1, name_length, infile);
            if(check2 < name_length) {
                perror("fread");
                exit(-1);
            }
            name[name_length] = '\0';
            set_inode( (ino_t)inode, name);
            read_size += name_length;

            // Get the file mode
            check2 = fread(&mode, 1, sizeof(int), infile);
            if(check2 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);

            // Directories
            if (S_ISDIR((mode_t)mode)) {
                // Get the file modification time
                check2 = fread(&time, 1, sizeof(long long int), infile);
                if(check2 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);
                // Create the directory
                mkdir(name, mode);
            } 
            else if (S_ISLNK((mode_t)mode)) {
                // Do Nothing
                // Executable files
            } 
            else {
                // Get the file modification time
                check2 = fread(&time, 1, sizeof(long long int), infile);
                if(check2 < sizeof(long long int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);

                // Get the file size
                check2 = fread(&file_size, 1, sizeof(long long int), infile);
                if(check2 < sizeof(int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += sizeof(long long int);

                // Create a new file
                FILE *newFile = fopen(name, "w+");
                if (newFile == NULL) {
                    perror("fopen");
                    exit(-1);
                }

                // Get the file content
                char *buff = (char *)malloc(sizeof(char)*(file_size+1));     
                check2 = fread(buff, 1, file_size, infile);
                if(check2 < sizeof(int)) {
                    perror("fread");
                    exit(-1);
                }
                read_size += file_size;
                buff[file_size] = '\0';
                // Write the file content
                check2 = fwrite(buff, 1, file_size, newFile);
                if (check2 < strlen(buff)) {
                    perror("fwrite");
                    exit(-1);
                }

                free(buff);
                fclose(newFile);
            }
            
        } 
        else {
            // Get the file name length
            check2 = fread(&name_length, 1, sizeof(int), infile);
            if(check2 < sizeof(int)) {
                perror("fread");
                exit(-1);
            }
            read_size += sizeof(int);

            char * name = (char *)malloc(sizeof(char)*(name_length+1));
            check2 = fread(name, 1, name_length, infile);
            if(check2 < name_length) {
                perror("fread");
                exit(-1);
            }
            name[name_length] = '\0';
            read_size += name_length;

            link(get_inode((ino_t)inode),name);
        }
        /* Make the recursive call if the file is a directory */
        // if (S_ISDIR(buf.st_mode)) {
        //     extract_content(output, fullname);
        // }
    }
    free(fullname);
    fclose(infile);
}