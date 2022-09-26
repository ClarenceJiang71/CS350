/* 
Clarence Jiang, yjia279
THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANYSOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. 
Clarence Jiang  */

//
//  inodemap.c
//  mytar
//
//  Created by Dorian Arnold on 10/8/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//

#include "inodemap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int magic_num= 0x7261746D;

char * no_tar = "Error: No tarfile specified.\n";
char * multiple_mode = "Error: Multiple modes specified.\n";
char * no_mode = "Error: No mode specified.\n";
char * target_no_exist = "Error: Specified target(\"%s\") does not exist.\n";
char * target_not_directory = "Error: Specified target(\"%s\") is not a directory.\n";
char * no_directory = "Error: No directory target specified.\n";
char * bad_magic = "Error: Bad magic number (%d), should be: %d.\n";


void create_tar(FILE * tar, char * directory, int recursion_tracker){
    struct stat buf;
    int exists = 0;
    struct dirent *de;
    char * fullname;
    int result;
    char * path_test; 
    

    DIR * folder = opendir(directory);
    if (folder == NULL){
        fprintf(stderr, target_not_directory, directory);
        exit(1);
    }

    fullname = (char *)malloc(sizeof(char)*(strlen(directory)+258));
    long long int inode_num;
    int file_name_length;
    int file_mode;
    long long int file_size;
    long long int file_modification_time;

    /*directory name preprocessing*/
    if (directory[strlen(directory)-1] == '/'){
        directory[strlen(directory)-1] = '\0';
    }
    
    /*deal with the root directory information*/
    if (recursion_tracker < 2){
         /*add magic number*/
        int result_mag = fwrite(&magic_num, 1, sizeof(int), tar);
        if (result_mag < sizeof(int)){
            perror("Error: fwrite");
            exit(1);
        }

        
        path_test = strrchr(directory, '/');
    
        if (path_test != NULL){
            char directory_processed [strlen(path_test)];
            memset(directory, '\0', sizeof(directory_processed));
            strncpy(directory, path_test+1, strlen(path_test)-1);
        }
    
        sprintf(fullname, "%s", directory);

        
        exists = lstat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found\n", fullname);
            exit(1);
        }
    
        if(!get_inode( buf.st_ino)) {
            set_inode( buf.st_ino, fullname);

            inode_num = (long long int) buf.st_ino;
            file_name_length = (int) strlen(fullname);
            file_mode = (int) buf.st_mode;
            file_modification_time = (long long int) buf.st_mtime;

            result = fwrite(&inode_num, 1, sizeof(long long int), tar);
            if (result < sizeof(long long int)){
                perror("Error: fwrite");
                exit(1);
            }
            result = fwrite(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fwrite");
                exit(1);
            }
            result = fwrite(fullname, 1, strlen(fullname), tar);
            if (result < strlen(fullname)){
                perror("Error: fwrite");
                exit(1);
            }
            result = fwrite(&file_mode, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fwrite");
                exit(1);
            }
            result = fwrite(&file_modification_time, 1, sizeof(long long int), tar);
            if (result < sizeof(long long int)){
                perror("Error: fwrite");
                exit(1);
            }
        }
    }

    

    for (de = readdir(folder); de != NULL; de = readdir(folder)) {
        sprintf(fullname, "%s/%s", directory, de->d_name);
        exists = lstat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found\n", fullname);
            exit(1);
        }

        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
            continue;
        }

        if(!get_inode( buf.st_ino)) {
            set_inode( buf.st_ino, fullname);

            inode_num = (long long int) buf.st_ino;
            file_name_length = (int) strlen(fullname);
            file_mode = (int) buf.st_mode;
            file_modification_time = (long long int) buf.st_mtime;

            if (S_ISDIR(buf.st_mode)) {
                result = fwrite(&inode_num, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_name_length, 1, sizeof(int), tar);
                if (result < sizeof(int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(fullname, 1, strlen(fullname), tar);
                if (result < strlen(fullname)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_mode, 1, sizeof(int), tar);
                if (result < sizeof(int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fwrite");
                    exit(1);
                }
            } 
            else if (S_ISREG(buf.st_mode)) {
                file_size = (long long int) buf.st_size;
                FILE * fd = fopen(fullname, "r");
                char buffer_flexible[file_size];

                result = fwrite(&inode_num, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_name_length, 1, sizeof(int), tar);
                if (result < sizeof(int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(fullname, 1, strlen(fullname), tar);
                if (result < strlen(fullname)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_mode, 1, sizeof(int), tar);
                if (result < sizeof(int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fwrite");
                    exit(1);
                }
                result = fwrite(&file_size, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fwrite");
                    exit(1);
                }

                if (fd == NULL)
                {
                    perror("Error: fopen");
                    exit(1);
                }

                fseek(fd, 0, SEEK_SET);
                result = fread(buffer_flexible, 1, file_size, fd);
                if (result < file_size){
                    perror("Error: fread");
                    exit(1);
                }
                result = fwrite(buffer_flexible, 1, file_size, tar);
                if (result < file_size){
                    perror("Error: fwrite");
                    exit(1);
                }
                fclose(fd);
            } 
        }
        else{
            inode_num = (long long int) buf.st_ino;
            result = fwrite(&inode_num, 1, sizeof(long long int), tar);
            if (result < sizeof(long long int)){
                perror("Error: fwrite");
                exit(1);
            }
            file_name_length = (int) strlen(fullname);
            result = fwrite(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fwrite");
                exit(1);
            }
            result = fwrite(fullname, 1, file_name_length, tar);
            if (result < file_name_length){
                perror("Error: fwrite");
                exit(1);
            }
        }

        if (S_ISDIR(buf.st_mode)) {
            recursion_tracker += 1;
            create_tar(tar, fullname, recursion_tracker);
        }
    }
    free(fullname);
    closedir(folder);
}

void print_tar(char * archive_name){
    FILE * tar = fopen(archive_name, "r");
    struct stat buf;
    int exists = 0;
    int result = 0;
    
    int check_magic_num;
    long long int inode_num;
    int file_name_length;
    int file_mode;
    long long int file_modification_time;
    long long int file_size;

    int count_size = 0;

    exists = lstat(archive_name, &buf);
    if (exists < 0){
        perror("Error: lstat");
        fclose(tar);
        exit(1);
    }

    /*check magic number*/
    result = fread(&check_magic_num, 1, sizeof(int), tar);
    if (result < sizeof(int)){
        perror("Error: fread");
        fclose(tar);
        exit(1);
    }
    if (check_magic_num != magic_num){
        fprintf(stderr, bad_magic, check_magic_num,magic_num);
        fclose(tar);
        exit(1);
    }
    count_size += sizeof(check_magic_num);

    /*read through tar file*/
    while(count_size < buf.st_size){

        result = fread(&inode_num, 1, sizeof(long long int), tar);
        if (result< sizeof(long long int)){
            perror("Error: fread");
            fclose(tar);
            exit(1);
        }
        count_size += sizeof(inode_num);

        if (!get_inode((ino_t) inode_num)){
            result = fread(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                fclose(tar);
                perror("Error: fread");
                exit(1);
            }
            count_size += sizeof(file_name_length);

            char file_name[file_name_length];
            result = fread(file_name, 1, file_name_length, tar);
            if (result< file_name_length){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += file_name_length;

            set_inode((ino_t)inode_num, file_name);
            result = fread(&file_mode, 1, sizeof(int), tar);
            if (result< sizeof(int)){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += sizeof(file_mode);


            if (S_ISDIR((mode_t)file_mode)){
                result = fread(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_modification_time);
                file_name[file_name_length] = '/';
                file_name[file_name_length+1] = '\0';
                printf("%s -- inode: %llu, mode: %o, mtime: %llu\n", file_name, inode_num, file_mode & 0x1ff, file_modification_time);
            }
            else if((mode_t)file_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
                result = fread(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_modification_time);
                result =fread(&file_size, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_size);
                if (fseek(tar, file_size, SEEK_CUR)!=0){
                    perror("Error: fseek");
                    fclose(tar);
                    exit(1);
                }
                count_size += file_size;
                file_name[file_name_length] = '*';
                file_name[file_name_length+1] = '\0';
                printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, file_mode & 0x1ff, file_modification_time, file_size);
            }
            else{
                result = fread(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_modification_time);
                result =fread(&file_size, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_size);
                if (fseek(tar, file_size, SEEK_CUR)!=0){
                    perror("Error: fseek");
                    fclose(tar);
                    exit(1);
                }
                count_size += file_size;
                file_name[file_name_length] = '\0';
                printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, file_mode & 0x1ff, file_modification_time, file_size);
            }
        }
        /*hard link*/
        else{
            result = fread(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += sizeof(file_name_length);
            char file_name[file_name_length];
            result = fread(file_name, 1, file_name_length, tar);
            if (result< file_name_length){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += file_name_length;
            printf("%s -- inode: %llu\n", file_name, inode_num);
        }
    }
    fclose(tar);
}

void extract_tar(char * archive_name){
    FILE * tar = fopen(archive_name, "r");
    struct stat buf;
    int exists = 0;
    int result = 0;
    
    int check_magic_num;
    long long int inode_num;
    int file_name_length;
    int file_mode;
    long long int file_modification_time;
    long long int file_size;
    char * file_name;

    int count_size = 0;

    exists = lstat(archive_name, &buf);
    if (exists < 0){
        perror("Error: lstat");
        fclose(tar);
        exit(1);
    }

    /*check magic number*/
    result = fread(&check_magic_num, 1, sizeof(int), tar);
    if (result < sizeof(int)){
        perror("Error: fread");
        fclose(tar);
        exit(1);
    }
    if (check_magic_num != magic_num){
        fprintf(stderr, bad_magic, check_magic_num,magic_num);
        fclose(tar);
        exit(1);
    }
    count_size += sizeof(check_magic_num);


    while(count_size < buf.st_size){

        result = fread(&inode_num, 1, sizeof(long long int), tar);
        if (result< sizeof(long long int)){
            perror("Error: fread");
            fclose(tar);
            exit(1);
        }
        count_size += sizeof(inode_num);

        if (!get_inode((ino_t) inode_num)){
            result = fread(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += sizeof(file_name_length);

            file_name = (char *) malloc(sizeof(char)*(file_name_length)+258);

            result = fread(file_name, 1, file_name_length, tar);
            if (result< file_name_length){
                perror("Error: fread");
                free(file_name);
                fclose(tar);
                exit(1);
            }
            count_size += file_name_length;

            file_name[file_name_length] = '\0';
            set_inode((ino_t)inode_num, file_name);
            result = fread(&file_mode, 1, sizeof(int), tar);
            if (result< sizeof(int)){
                perror("Error: fread");
                free(file_name);
                fclose(tar);
                exit(1);
            }
            count_size += sizeof(file_mode);


            if (S_ISDIR((mode_t)file_mode)){
                result = fread(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    fclose(tar);
                    free(file_name);
                    exit(1);
                }
                count_size += sizeof(file_modification_time);
                file_name[file_name_length] = '\0';
                result = mkdir(file_name, (mode_t) file_mode);
                if(result == -1){
                    perror("Error: mkdir");
                    free(file_name);
                    fclose(tar);
                    exit(1);
                }

                /*time adjustment*/
                struct timeval time[2];
                result = gettimeofday(&time[0], NULL);
                if(result == -1){
                    perror("Error: gettimeofday");
                    free(file_name);
                    fclose(tar);
                    exit(1);
                }
                time[1].tv_sec = file_modification_time;
                time[1].tv_usec = 0;
                result = utimes(file_name, time);
                if(result == -1){
                    perror("Error: utimes");
                    free(file_name);
                    fclose(tar);
                    exit(1);
                }

            }
            else if(S_ISREG((mode_t)file_mode)){
                result = fread(&file_modification_time, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    free(file_name);
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_modification_time);
                result =fread(&file_size, 1, sizeof(long long int), tar);
                if (result < sizeof(long long int)){
                    perror("Error: fread");
                    free(file_name);
                    fclose(tar);
                    exit(1);
                }
                count_size += sizeof(file_size);

                //create file
                file_name[file_name_length] = '\0';

                FILE * add_file = fopen(file_name, "w+");
                 
                char buffer_flexible[file_size];
                result = fread(buffer_flexible, 1, file_size, tar);
                if (result < file_size){
                    perror("Error: fread");
                    free(file_name);
                    fclose(tar);
                    fclose(add_file);
                    exit(1);
                }
                count_size += file_size;

                result = fwrite(buffer_flexible, 1, file_size, add_file);
                if (result < file_size){
                    perror("Error: fwrite");
                    free(file_name);
                    fclose(tar);
                    fclose(add_file);
                    exit(1);
                }
                fclose(add_file);

                if((mode_t)file_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
                    result = chmod(file_name, S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH|S_IRWXU);
                    if(result == -1){
                        perror("Error: chmod");
                        free(file_name);
                        fclose(tar);
                        exit(1);
                    }
                }

                /*time adjustment*/
                struct timeval time[2];
                result = gettimeofday(&time[0], NULL);
                if(result == -1){
                    perror("Error: gettimeofday");
                    free(file_name);
                    fclose(tar);
                    fclose(add_file);
                    exit(1);
                }
                time[1].tv_sec = file_modification_time;
                time[1].tv_usec = 0;
                result = utimes(file_name, time);
                if(result == -1){
                    perror("Error: utimes");
                    free(file_name);
                    fclose(tar);
                    fclose(add_file);
                    exit(1);
                }
            }
        }
        /*hard link*/
        else{
            result = fread(&file_name_length, 1, sizeof(int), tar);
            if (result < sizeof(int)){
                perror("Error: fread");
                fclose(tar);
                exit(1);
            }
            count_size += sizeof(file_name_length);
            file_name = (char *) malloc(sizeof(char) * (file_name_length) + 258 );
            result = fread(file_name, 1, file_name_length, tar);
            if (result< file_name_length){
                perror("Error: fread");
                free(file_name);
                fclose(tar);
                exit(1);
            }
            count_size += file_name_length;
            file_name[file_name_length] = '\0';
            result = link(get_inode((ino_t) inode_num), file_name);
            if (result < 0){
                perror("Error: link");
                free(file_name);
                fclose(tar);
                exit(1);
            }
    
        }
    }
    free(file_name);
    fclose(tar);

}


int main(int argc, char * argv[]){
    /*
    Create useful local variables in the beginning, since creating it within the branch of while loop
    seems not working. 
    */
    int o; //keep track of option
    char option = 'i'; //keep track of the option that we enter in.  
    int opt_count = 0;
    char * archive_name; 
    char * directory;
    int recursion_tracker = 1;
    FILE * tar;
    int exists;
    struct stat buf;
    int check_f = 0;
 
    //The first while loop is created to extract different parts ot the input command 
    while ((o = getopt(argc, argv, "cxtf:")) != -1) {
        switch (o) {
            case 'f':
                check_f = 1;
                archive_name = optarg;
                break;
            case 'c':
                option = 'c';
                opt_count++;
                break;
            case 'x':
                option = 'x';
                opt_count++;
                break;
            case 't':
                option = 't';
                opt_count++;
                break;
            default: 
                fprintf(stderr, "%s", no_tar);
                exit(1);
                break;
        }
    }

    if (option == 'i'){
        fprintf(stderr, "%s", no_mode);
        exit(1);
    }

    if (check_f == 0){
        fprintf(stderr, "%s", no_tar);
        exit(1);
    }

    if (opt_count>1){
        fputs(multiple_mode, stderr);
        exit(1);
    }

    directory = argv[optind];
    
    switch (option) {
        case 'c':
            if (directory == NULL){
                fprintf(stderr, "%s", no_directory);
                exit(1);
            }
            tar = fopen(archive_name, "w+");
            exists = lstat(archive_name, &buf);
            if (exists < 0)
            {
                perror("Error: lstat");
                exit(1);
            }
            
            DIR * result = opendir(directory);
            if (result == NULL){
                fprintf(stderr, target_not_directory, directory);
                exit(1);
            }

            create_tar(tar, directory, recursion_tracker);
            fclose(tar);
            break;
        case 'x':
            tar = fopen(archive_name, "r");
            exists = lstat(archive_name, & buf);
            if (exists<0){
                fprintf(stderr, target_no_exist, archive_name);
                exit(1);
            }
            extract_tar(archive_name);
            break;
        case 't':
            tar = fopen(archive_name, "r");
            exists = lstat(archive_name, & buf);
            if (exists<0){
                fprintf(stderr, target_no_exist, archive_name);
                exit(1);
            }
            print_tar(archive_name);
            break;
        case 'f':
            break;
        default: /* '?' */
            exit(1);
            break;
    }
    
    return 0;
}



