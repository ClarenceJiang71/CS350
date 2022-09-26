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

// Define Option struct to organize the options:
// transCount is for recording the number of modes specified, in case of multiple mode error;
// transIndic is for signaling what mode should be performed;
typedef struct {
    int transCount;
    int transIndic;
} Option;

// Helper function declarations.
void createArchive( FILE * tarfd, char * tardirename );
void extract( char * tarfilename );
void printArchive( char * tarfilename );

// Record the inode of the target tar file after its creation
int tarinode;

char magicNumber[] = "0x7261746D\n";

int main( int argc, char *argv[] ) {
  Option *newop;
  newop = malloc(sizeof(Option));
  // out is to record whether -f, which is mandatory, presents.
  int out = -1;
  int o1;
  // To record the name of the target tar file
  char * tarfilename;
  // To record the name of the target directory
  char * tardirename;
  // To record the stat of the target tar file
  struct stat buf0;
  int exists0;

  // getopt to capture the options.
  while ((o1 = getopt(argc, argv, "cxtf")) != -1) {
      switch (o1) {
      default:
          fprintf(stderr, "Error:  No mode specified. \n");
          free(newop);
          exit(-1);
          break;
      case 'f':
          out = 1;
          break;
      case 'c':
          newop->transIndic = 0;
          newop->transCount++;
          break;
      case 'x':
          newop->transIndic = 1;
          newop->transCount++;
          break;
      case 't':
          newop->transIndic = 2;
          newop->transCount++;
          break;
      }
  }

  if (newop->transCount == 0) {
    fprintf(stderr, "Error:  No mode specified. \n");
    free(newop);
    exit(-1);
  }
  if (newop->transCount > 1) {
    fprintf(stderr, "Error:  Multiple modes specified. \n");
    free(newop);
    exit(-1);
  }
  if (out == -1) {
    fprintf(stderr, "Error:  No tarfile specified. \n");
    free(newop);
    exit(-1);
  }
  // Preliminary checks passed and start to branch into different fates
  if (newop->transIndic == 0) {
      if (strcmp(argv[argc-3], "-f") != 0) {
          fprintf(stderr, "Error:  No tarfile specified. \n");
          free(newop);
          exit(-1);
      }
      tarfilename = argv[argc-2];
      tardirename = argv[argc-1];
      FILE *tarfd;
      // Create and open the target tar file.
      tarfd = fopen(tarfilename, "w+");
      exists0 = lstat(tarfilename, &buf0);
      if (exists0 < 0) {
        perror("lstat");
        exit(-1);
      }
      tarinode = (int)buf0.st_ino;
      // Write the magic number first! Tarsonal identification.
      int check0 = fwrite(magicNumber, 1, strlen(magicNumber), tarfd);
      if (check0 < strlen(magicNumber)) {
        perror("fwrite");
        exit(-1);
      }
      // Here is the main dish
      createArchive(tarfd, tardirename);
      fclose(tarfd);
  } else if (newop->transIndic == 1) {
      if (argc != 4) {
          fprintf(stderr, "Error:  No tarfile specified. \n");
          free(newop);
          exit(-1);
      }
      tarfilename = argv[3];
      // Here is the main dish x2
      extract(tarfilename);
  } else {
      if (argc != 4) {
          fprintf(stderr, "Error:  No tarfile specified. \n");
          free(newop);
          exit(-1);
      }
      tarfilename = argv[3];
      // Here is the main dish x3
      printArchive(tarfilename);
  }
  return 0;
}




// Helper Functions.
// The first one: for option -c: create an archive from a directory tree
void createArchive( FILE * tarfd, char * tardirename ) {
  DIR *d;
  struct dirent *de;
  struct stat buf;
  int exists;
  char * fullname;
  // The transfer station for writing to the tar file
  char * textTransfer;
  // An Int for checking system and library calls for failure
  int check;

  d = opendir(tardirename);
  if (d == NULL) {
      fprintf(stderr, "Specified target %s is not a directory. \n", tardirename);
      exit(-1);
  }

  // Begin to traverse the directory tree and archive all files, directories, and hard links
  fullname = (char *)malloc(sizeof(char)*(strlen(tardirename)+258));
  textTransfer = (char *)malloc(sizeof(char)*100000);

  for (de = readdir(d); de != NULL; de = readdir(d)) {
      sprintf(fullname, "%s/%s", tardirename, de->d_name);

      exists = lstat(fullname, &buf);
      if (exists < 0) {
        perror("lstat");
        exit(-1);
      }
      // Ignore ., .., mytar, and the target tar file
      if (strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 && buf.st_ino != (ino_t)tarinode && strcmp(de->d_name, "mytar") !=0) {
        if( !get_inode( buf.st_ino ) ) {
            //inode not yet seen; add to list and process
            set_inode( buf.st_ino, fullname );
            // Directories
            if (S_ISDIR(buf.st_mode)) {
              // Record file inode number
              sprintf(textTransfer, "%llu\n", (long long int)buf.st_ino);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file name length
              sprintf(textTransfer, "%d\n", (int)strlen(fullname));
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file name
              sprintf(textTransfer, "%s\n", fullname);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file mode
              sprintf(textTransfer, "%o\n", (int)buf.st_mode);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file modification time
              sprintf(textTransfer, "%s", ctime(&buf.st_mtime));
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
            } else if (S_ISLNK(buf.st_mode)) {
                // Do nothing - ignore soft links
            // Regular Files
            } else {
              // Record file inode number
              sprintf(textTransfer, "%llu\n", (long long int)buf.st_ino);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file name length
              sprintf(textTransfer, "%d\n", (int)strlen(fullname));
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file name
              sprintf(textTransfer, "%s\n", fullname);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file mode
              sprintf(textTransfer, "%o\n", (int)buf.st_mode);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file modification time
              sprintf(textTransfer, "%s", ctime(&buf.st_mtime));
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // Record file size
              sprintf(textTransfer, "%llu\n", (long long int)buf.st_size);
              check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
              if (check < strlen(textTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              //  Record file content
              FILE *fd = fopen(fullname, "r");
              if (fd == NULL) {
                perror("fopen");
                exit(-1);
              }
              char *contentTransfer = (char *)malloc(sizeof(char)*((int)buf.st_size));
              check = (int)fread(contentTransfer, 1, (int)buf.st_size-1, fd);
              if (check < (int)buf.st_size-1) {
                  perror("fread");
                  exit(-1);
              }
              contentTransfer[strlen(contentTransfer)] = '\n';
              check = fwrite(contentTransfer, 1, strlen(contentTransfer), tarfd);
              if (check < strlen(contentTransfer)) {
                perror("fwrite");
                exit(-1);
              }
              // check = fseek(fd, (int)buf.st_size-2, SEEK_SET);
              // if (check != 0) {
              //   perror("fseek");
              //   exit(-1);
              // }
              // if ( fgets(contentTransfer, 2, fd)==NULL ) {
              //   perror("fgets");
              //   exit(-1);
              // }
              // check = fwrite(contentTransfer, 1, 2, tarfd);
              // if (check < 2) {
              //   perror("fwrite");
              //   exit(-1);
              // }
              free(contentTransfer);
              fclose(fd);
            }
          // Hard Links
        } else {
          // Record file inode number
          sprintf(textTransfer, "%llu\n", (long long int)buf.st_ino);
          check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
          if (check < strlen(textTransfer)) {
            perror("fwrite");
            exit(-1);
          }
          // Record file name length
          sprintf(textTransfer, "%d\n", (int)strlen(fullname));
          check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
          if (check < strlen(textTransfer)) {
            perror("fwrite");
            exit(-1);
          }
          // Record file name
          sprintf(textTransfer, "%s\n", fullname);
          check = fwrite(textTransfer, 1, strlen(textTransfer), tarfd);
          if (check < strlen(textTransfer)) {
            perror("fwrite");
            exit(-1);
          }
        }

        /* Make the recursive call if the file is a directory */
        if (S_ISDIR(buf.st_mode)) {
              createArchive(tarfd, fullname);
        }
      }
  }
  closedir(d);
}

// The second one: for option -x: extract the directory tree contained in the archive
void extract( char * tarfilename ) {
  int z = 0;
}

// The third one: for option -t: print the contents of the specified archive
void printArchive( char * tarfilename ) {
  FILE *tarfd2;
  struct stat buf2;
  int check2;
  char *contentTransfer2 = (char *)malloc(sizeof(char)*100000);
  char *inodeText = (char *)malloc(sizeof(char)*100);
  char *fnameText = (char *)malloc(sizeof(char)*1000);
  char *fmodeText = (char *)malloc(sizeof(char)*1000);
  char *mtimeText = (char *)malloc(sizeof(char)*100);
  char *fsizeText = (char *)malloc(sizeof(char)*100);
  // Open the target tar file for reading.
  tarfd2 = fopen(tarfilename, "r");
  int exists2 = lstat(tarfilename, &buf2);
  if (exists2 < 0) {
    perror("lstat");
    exit(-1);
  }
  // Check if the specified target is a tar file
  check2 = (int)fread(contentTransfer2, 1, 11, tarfd2);
  if (check2 < 11) {
      perror("fread");
      exit(-1);
  }
  if (strcmp(contentTransfer2, "0x7261746D\n") !=0) {
    fprintf(stderr, "Bad magic number\n");
    exit(-1);
  }
  while(!feof(tarfd2)) {
    // Get the inode number
    if ( fgets(inodeText, (int)buf2.st_size, tarfd2)==NULL ) {
      perror("fgets");
      exit(-1);
    }
    int inode = atoi(inodeText);

    if( !get_inode((ino_t)inode) ) {
      set_inode( (ino_t)inode, inodeText );
      // Get the file name length
      if ( fgets(contentTransfer2, (int)buf2.st_size, tarfd2)==NULL ) {
        perror("fgets");
        exit(-1);
      }
      // Get the file name
      if ( fgets(fnameText, (int)buf2.st_size, tarfd2)==NULL ) {
        perror("fgets");
        exit(-1);
      }
      // Get the file mode
      if ( fgets(fmodeText, (int)buf2.st_size, tarfd2)==NULL ) {
        perror("fgets");
        exit(-1);
      }
      int fmode = atoi(fmodeText);

      // Directories
      if (S_ISDIR((mode_t)fmode)) {
        // Get the file modification time
        if ( fgets(mtimeText, (int)buf2.st_size, tarfd2)==NULL ) {
          perror("fgets");
          exit(-1);
        }
        mtimeText[strlen(mtimeText)-1] = '\0';
        fnameText[strlen(fnameText)-1] = '/';
        printf("%s inode: %llu, mode: %o, mtime: %s\n", fnameText, (long long int)inode, fmode, mtimeText);
        // Soft links (possibly not exist)
      } else if (S_ISLNK((mode_t)fmode)) {
        // Do Nothing
        // Executable files
      } else if ((mode_t)fmode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        // Get the file modification time
        if ( fgets(mtimeText, (int)buf2.st_size, tarfd2)==NULL ) {
          perror("fgets");
          exit(-1);
        }
        mtimeText[strlen(mtimeText)-1] = '\0';
        // Get the file size
        if ( fgets(fsizeText, (int)buf2.st_size, tarfd2)==NULL ) {
          perror("fgets");
          exit(-1);
        }
        int fsize = atoi(fsizeText);
        check2 = fseek(tarfd2, fsize, SEEK_CUR);
        if (check2 != 0) {
          perror("fseek");
          exit(-1);
        }
        fnameText[strlen(fnameText)-1] = '*';
        printf("%s inode: %llu, mode: %o, mtime: %s, size: %llu\n", fnameText, (long long int)inode, fmode, mtimeText, (long long int)fsize);
        // Regular files
      } else {
        // Get the file modification time
        if ( fgets(mtimeText, (int)buf2.st_size, tarfd2)==NULL ) {
          perror("fgets");
          exit(-1);
        }
        mtimeText[strlen(mtimeText)-1] = '\0';
        // Get the file size
        if ( fgets(fsizeText, (int)buf2.st_size, tarfd2)==NULL ) {
          perror("fgets");
          exit(-1);
        }
        int fsize = atoi(fsizeText);
        check2 = fseek(tarfd2, fsize, SEEK_CUR);
        if (check2 != 0) {
          perror("fseek");
          exit(-1);
        }
        fnameText[strlen(fnameText)-1] = '\0';
        printf("%s inode: %llu, mode: %o, mtime: %s, size: %llu\n", fnameText, (long long int)inode, fmode, mtimeText, (long long int)fsize);
      }
      // Hard links
    } else {
      // Get the file name length
      if ( fgets(contentTransfer2, (int)buf2.st_size, tarfd2)==NULL ) {
        perror("fgets");
        exit(-1);
      }
      // Get the file name
      if ( fgets(fnameText, (int)buf2.st_size, tarfd2)==NULL ) {
        perror("fgets");
        exit(-1);
      }
      fnameText[strlen(fnameText)-1] = '/';
      printf("%s inode: %llu\n", fnameText, (long long int)inode);
    }
  }
  fclose(tarfd2);
}
