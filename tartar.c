#include "inodemap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>

// Define Option struct to organize the options:
// transCount is for recording the number of modes specified, in case of multiple mode error;
// transIndic is for signaling what mode should be performed;
typedef struct {
    int transCount;
    int transIndic;
} Option;

// Record the inode of the target tar file after its creation
int tarinode;

// Helper function declarations.
void createArchive( FILE * tarfd, char * tardirename, int level, int start );
void extract( char * tarfilename );
void printArchive( char * tarfilename );

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
  // Preliminary checks passed and start to sail to different fates
  int fate;
  if (newop->transIndic == 0) {
    fate = 0;
  } else if (newop->transIndic == 1) {
    fate = 1;
  } else {
    fate = 2;
  }
  free(newop);

  if (fate == 0) {
    if (argc < 5) {
        fprintf(stderr, "Error:  No directort target specified. \n");
        exit(-1);
    }
    if (strcmp(argv[argc-3], "-f") != 0) {
        fprintf(stderr, "Error:  No tarfile specified. \n");
        exit(-1);
    }
    tarfilename = argv[argc-2];
    tardirename = argv[argc-1];
    // Create and open the target tar file.
    FILE *tarfd = fopen(tarfilename, "w+");
    if (lstat(tarfilename, &buf0) < 0) {
      perror("lstat");
      exit(-1);
    }
    tarinode = (int)buf0.st_ino;
    // Write the magic number first! Tarsonal identification.
    int *magicNumber = (int *)malloc(sizeof(int));
    magicNumber[0] = 0x7261746D;
    if (fwrite(magicNumber, 1, sizeof(int), tarfd) < sizeof(int)) {
      perror("fwrite");
      free(magicNumber);
      exit(-1);
    }
    free(magicNumber);


    const char signal = '/';
    int printStart = 0;
    if (strchr(tardirename, signal) != NULL) {
      if (strcmp(&tardirename[strlen(tardirename)-1], "/") == 0 && strcmp(&tardirename[strlen(tardirename)-2], "/") == 0) {
        fprintf(stderr, "Specified target %s is not a directory. \n", tardirename);
        exit(-1);
      }
      if (strcmp(&tardirename[strlen(tardirename)-1], "/") == 0) {
        tardirename[strlen(tardirename)-1] = '\0';
      }
      char *after;
      after = strrchr(tardirename, signal);
      printStart = strlen(tardirename) - strlen(after) + 1;
    }

    // Here is the main dish
    int level = 0;
    createArchive(tarfd, tardirename, level, printStart);
    fclose(tarfd);
  } else if (fate == 1) {
    if (argc < 4) {
        fprintf(stderr, "Error:  No tarfile specified. \n");
        exit(-1);
    }
    tarfilename = argv[3];
    // Here is the main dish x2
    extract(tarfilename);
  } else {
    if (argc < 4) {
        fprintf(stderr, "Error:  No tarfile specified. \n");
        exit(-1);
    }
    tarfilename = argv[3];
    // Here is the main dish x3
    printArchive(tarfilename);
  }
  return 0;
}



// Helper function.
// The first one: for option -c: create an archive from a directory tree
void createArchive( FILE * tarfd, char * tardirename, int level, int start ) {
  DIR *d;
  struct dirent *de;
  struct stat buf;
  char * fullname = (char *)malloc(sizeof(char)*(strlen(tardirename)+258));

  d = opendir(tardirename);
  if (d == NULL) {
      fprintf(stderr, "Specified target %s is not a directory. \n", tardirename);
      free(fullname);
      exit(-1);
  }
  // Begin to traverse the directory tree and archive all files, directories, and hard links
  for (de = readdir(d); de != NULL; de = readdir(d)) {
    sprintf(fullname, "%s/%s", tardirename, de->d_name);
    if (lstat(fullname, &buf) < 0) {
      perror("lstat");
      free(fullname);
      exit(-1);
    }
    // Record the information of the initial directory passed as parameter
    if (strcmp(de->d_name, ".") ==0 && level==0) {
      // Record file inode number
      long long int *inodeNumber = (long long int *)malloc(sizeof(long long int));
      inodeNumber[0] = (long long int)buf.st_ino;
      if (fwrite(inodeNumber, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
        perror("fwrite");
        free(fullname);
        free(inodeNumber);
        exit(-1);
      }
      free(inodeNumber);
      // Record file name length
      int *nameLength = (int *)malloc(sizeof(int));
      nameLength[0] = (int)strlen(tardirename) - start;
      if (fwrite(nameLength, 1, sizeof(int), tarfd) < sizeof(int)) {
        perror("fwrite");
        free(fullname);
        free(nameLength);
        exit(-1);
      }
      free(nameLength);
      // Record file name
      char *NameCarried = (char *)malloc(sizeof(char)*((int)strlen(tardirename) - start + 1));
      NameCarried[(int)strlen(tardirename) - start] = '\0';
      for (int i=start; i<(int)strlen(tardirename); i++) {
        NameCarried[i - start] = tardirename[i];
      }

      if (fwrite(NameCarried, 1, strlen(NameCarried), tarfd) < strlen(NameCarried)) {
        perror("fwrite");
        free(fullname);
        free(NameCarried);
        exit(-1);
      }
      free(NameCarried);
      // Record file mode
      int *fmode = (int *)malloc(sizeof(int));
      fmode[0] = (int)buf.st_mode;
      if (fwrite(fmode, 1, sizeof(int), tarfd) < sizeof(int)) {
        perror("fwrite");
        free(fullname);
        free(fmode);
        exit(-1);
      }
      free(fmode);
      // Record file modification time
      long long int *mtime = (long long int *)malloc(sizeof(long long int));
      mtime[0] = (long int)buf.st_mtime;
      if (fwrite(mtime, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
        perror("fwrite");
        free(mtime);
        free(fullname);
        exit(-1);
      }
      free(mtime);
    }
    // Ignore ., .., and the target tar file
    if (strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 && buf.st_ino != (ino_t)tarinode) {
      if( !get_inode( buf.st_ino ) ) {
        //inode not yet seen; add to list and process
        set_inode( buf.st_ino, fullname );
        // Directories
        if (S_ISDIR(buf.st_mode)) {
          // Record file inode number
          long long int *inodeNumber = (long long int *)malloc(sizeof(long long int));
          inodeNumber[0] = (long long int)buf.st_ino;
          if (fwrite(inodeNumber, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
            perror("fwrite");
            free(fullname);
            free(inodeNumber);
            exit(-1);
          }
          free(inodeNumber);
          // Record file name length
          int *nameLength = (int *)malloc(sizeof(int));
          nameLength[0] = (int)strlen(fullname) - start;
          if (fwrite(nameLength, 1, sizeof(int), tarfd) < sizeof(int)) {
            perror("fwrite");
            free(fullname);
            free(nameLength);
            exit(-1);
          }
          free(nameLength);
          // Record file name
          char *NameCarried = (char *)malloc(sizeof(char)*((int)strlen(fullname) - start + 1));
          NameCarried[(int)strlen(fullname) - start] = '\0';
          for (int i=start; i<(int)strlen(fullname); i++) {
            NameCarried[i - start] = fullname[i];
          }

          if (fwrite(NameCarried, 1, strlen(NameCarried), tarfd) < strlen(NameCarried)) {
            perror("fwrite");
            free(fullname);
            free(NameCarried);
            exit(-1);
          }
          free(NameCarried);
          // Record file mode
          int *fmode = (int *)malloc(sizeof(int));
          fmode[0] = (int)buf.st_mode;
          if (fwrite(fmode, 1, sizeof(int), tarfd) < sizeof(int)) {
            perror("fwrite");
            free(fullname);
            free(fmode);
            exit(-1);
          }
          free(fmode);
          // Record file modification time
          long long int *mtime = (long long int *)malloc(sizeof(long long int));
          mtime[0] = (long int)buf.st_mtime;
          if (fwrite(mtime, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
            perror("fwrite");
            free(mtime);
            free(fullname);
            exit(-1);
          }
          free(mtime);
        } else if (S_ISLNK(buf.st_mode)) {
          // Do nothing - ignore soft links
        // Regular Files
      } else {
        // Record file inode number
        long long int *inodeNumber = (long long int *)malloc(sizeof(long long int));
        inodeNumber[0] = (long long int)buf.st_ino;
        if (fwrite(inodeNumber, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
          perror("fwrite");
          free(fullname);
          free(inodeNumber);
          exit(-1);
        }
        free(inodeNumber);
        // Record file name length
        int *nameLength = (int *)malloc(sizeof(int));
        nameLength[0] = (int)strlen(fullname)-start;
        if (fwrite(nameLength, 1, sizeof(int), tarfd) < sizeof(int)) {
          perror("fwrite");
          free(fullname);
          free(nameLength);
          exit(-1);
        }
        free(nameLength);
        // Record file name
        char *NameCarried = (char *)malloc(sizeof(char)*((int)strlen(fullname) - start + 1));
        NameCarried[(int)strlen(fullname) - start] = '\0';
        for (int i=start; i<(int)strlen(fullname); i++) {
          NameCarried[i - start] = fullname[i];
        }

        if (fwrite(NameCarried, 1, strlen(NameCarried), tarfd) < strlen(NameCarried)) {
          perror("fwrite");
          free(fullname);
          free(NameCarried);
          exit(-1);
        }
        free(NameCarried);
        // Record file mode
        int *fmode = (int *)malloc(sizeof(int));
        fmode[0] = (int)buf.st_mode;
        if (fwrite(fmode, 1, sizeof(int), tarfd) < sizeof(int)) {
          perror("fwrite");
          free(fullname);
          free(fmode);
          exit(-1);
        }
        free(fmode);
        // Record file modification time
        long long int *mtime = (long long int *)malloc(sizeof(long long int));
        mtime[0] = (long long int)buf.st_mtime;
        if (fwrite(mtime, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
          perror("fwrite");
          free(mtime);
          free(fullname);
          exit(-1);
        }
        free(mtime);
        // Record file size
        long long int *fsize = (long long int *)malloc(sizeof(long long int));
        fsize[0] = (long long int)buf.st_size;
        if (fwrite(fsize, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
          perror("fwrite");
          free(fsize);
          free(fullname);
          exit(-1);
        }
        free(fsize);
        //  Record file content
        FILE *fd = fopen(fullname, "r");
        if (fd == NULL) {
          perror("fopen");
          free(fullname);
          exit(-1);
        }
        char contentTransfer[(int)buf.st_size];
        if ((int)fread(contentTransfer, 1, (int)buf.st_size, fd) < (int)buf.st_size) {
            perror("fread");
            free(fullname);
            exit(-1);
        }
        if (fwrite(contentTransfer, 1, (int)buf.st_size, tarfd) < (int)buf.st_size) {
          perror("fwrite");
          free(fullname);
          exit(-1);
        }
        fclose(fd);
      }
      // Hard Links
    } else {
      // Record file inode number
      long long int *inodeNumber = (long long int *)malloc(sizeof(long long int));
      inodeNumber[0] = (long long int)buf.st_ino;
      if (fwrite(inodeNumber, 1, sizeof(long long int), tarfd) < sizeof(long long int)) {
        perror("fwrite");
        free(fullname);
        free(inodeNumber);
        exit(-1);
      }
      free(inodeNumber);
      // Record file name length
      int *nameLength = (int *)malloc(sizeof(int));
      nameLength[0] = (int)strlen(fullname)-start;
      if (fwrite(nameLength, 1, sizeof(int), tarfd) < sizeof(int)) {
        perror("fwrite");
        free(fullname);
        free(nameLength);
        exit(-1);
      }
      free(nameLength);
      // Record file name
      char *NameCarried = (char *)malloc(sizeof(char)*((int)strlen(fullname) - start + 1));
      NameCarried[(int)strlen(fullname) - start] = '\0';
      for (int i=start; i<(int)strlen(fullname); i++) {
        NameCarried[i - start] = fullname[i];
      }

      if (fwrite(NameCarried, 1, strlen(NameCarried), tarfd) < strlen(NameCarried)) {
        perror("fwrite");
        free(fullname);
        free(NameCarried);
        exit(-1);
      }
      free(NameCarried);
    }
    /* Make the recursive call if the file is a directory */
    if (S_ISDIR(buf.st_mode)) {
          level++;
          createArchive(tarfd, fullname, level, start);
    }
    }
  }
  free(fullname);
  closedir(d);
}






// The second one: for option -x: extract the directory tree contained in the archive
void extract( char * tarfilename ) {
  // Int total for keeping track of the number of bytes read so that an end can be anticipated.
  int total;
  struct stat buf2;
  // Open the target tar file for reading.
  FILE *tarfd2 = fopen(tarfilename, "r");
  if (lstat(tarfilename, &buf2) < 0) {
    perror("lstat");
    exit(-1);
  }
  // Check if the specified target is a tar file
  int *magicChecker = (int *)malloc(sizeof(int));
  if ((int)fread(magicChecker, 1, sizeof(int), tarfd2) < sizeof(int)) {
      perror("fread");
      free(magicChecker);
      exit(-1);
  }
  if (magicChecker[0] != 0x7261746D) {
    fprintf(stderr, "Bad magic number\n");
    free(magicChecker);
    exit(-1);
  }
  free(magicChecker);
  total = total + (int)sizeof(int);
  // Check Passed!
  char *fnameText;
  while(total < (int)buf2.st_size) {
    // Get the inode number
    long long int *inodeGet = (long long int *)malloc(sizeof(long long int));
    if ( (int)fread(inodeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
      perror("fread");
      free(inodeGet);
      exit(-1);
    }
    int inode = inodeGet[0];
    free(inodeGet);
    total = total + (int)sizeof(long long int);

    if( !get_inode((ino_t)inode) ) {
      // Get the file name length
      int *nlenGet = (int *)malloc(sizeof(int));
      if ( (int)fread(nlenGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(nlenGet);
        exit(-1);
      }
      int nameLen = nlenGet[0];
      free(nlenGet);
      total = total + (int)sizeof(int);
      // Get the file name
      fnameText = (char *)malloc(sizeof(char)*1000);
      if ( (int)fread(fnameText, 1, nameLen, tarfd2) < nameLen ) {
        perror("fread");
        free(fnameText);
        exit(-1);
      }
      total = total + nameLen;
      // Get the file mode
      int *modeGet = (int *)malloc(sizeof(int));
      if ( (int)fread(modeGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(fnameText);
        free(modeGet);
        exit(-1);
      }
      int fmode = modeGet[0];
      free(modeGet);
      total = total + (int)sizeof(int);
      fnameText[nameLen] = '\0';
      set_inode( (ino_t)inode, fnameText );


      // Directories
      if (S_ISDIR((mode_t)fmode)) {
        // Get the file modification time
        long long int *timeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(timeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(timeGet);
          exit(-1);
        }
        int mtime = timeGet[0];
        free(timeGet);
        total = total + (int)sizeof(long long int);
        fnameText[nameLen] = '\0';
        if ( mkdir(fnameText, (mode_t)fmode) < 0 ) {
          perror("mkdir");
          free(fnameText);
          exit(-1);
        }
        struct timeval *timeArray = malloc(sizeof(struct timeval)*2);
        if (gettimeofday(&timeArray[0], NULL) < 0) {
          perror("gettimeofday");
          free(fnameText);
          free(timeArray);
          exit(-1);
        }
        timeArray[1].tv_sec = mtime;
        timeArray[1].tv_usec = 0;
        if (utimes(fnameText, timeArray) < 0) {
          perror("utimes");
          free(fnameText);
          free(timeArray);
          exit(-1);
        }
        free(timeArray);
        // Soft links (possibly not exist)
      } else if (S_ISLNK((mode_t)fmode)) {
        // Do Nothing
        // Regular files
      } else {
        // Get the file modification time
        long long int *timeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(timeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(timeGet);
          exit(-1);
        }
        int mtime = timeGet[0];
        free(timeGet);
        total = total + (int)sizeof(long long int);
        // Get the file size
        long long int *sizeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(sizeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(sizeGet);
          exit(-1);
        }
        int fsize = sizeGet[0];
        free(sizeGet);
        total = total + (int)sizeof(long long int);
        char *content = (char *)malloc(sizeof(char)*fsize);
        if ( (int)fread(content, 1, fsize, tarfd2) < fsize ) {
          perror("fread");
          free(fnameText);
          free(content);
          exit(-1);
        }
        total = total + fsize;
        fnameText[nameLen] = '\0';
        FILE *xfd = fopen(fnameText, "w+");
        if (fwrite(content, 1, fsize, xfd) < fsize) {
          perror("fwrite");
          free(fnameText);
          free(content);
          exit(-1);
        }
        free(content);
        struct timeval *timeArray = malloc(sizeof(struct timeval)*2);
        if (gettimeofday(&timeArray[0], NULL) < 0) {
          perror("gettimeofday");
          free(fnameText);
          free(timeArray);
          exit(-1);
        }
        timeArray[1].tv_sec = mtime;
        timeArray[1].tv_usec = 0;
        if (utimes(fnameText, timeArray) < 0) {
          perror("utimes");
          free(fnameText);
          free(timeArray);
          exit(-1);
        }
        free(timeArray);
      }
      // Hard links
    } else {
      // Get the file name length
      int *nlenGet = (int *)malloc(sizeof(int));
      if ( (int)fread(nlenGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(nlenGet);
        exit(-1);
      }
      int nameLen = nlenGet[0];
      free(nlenGet);
      total = total + (int)sizeof(int);
      // Get the file name
      fnameText = (char *)malloc(sizeof(char)*1000);
      if ( (int)fread(fnameText, 1, nameLen, tarfd2) < nameLen ) {
        perror("fread");
        free(fnameText);
        exit(-1);
      }
      total = total + nameLen;
      fnameText[nameLen] = '\0';
      if ( link(get_inode( (ino_t)inode ), fnameText) < 0 ) {
        perror("link");
        free(fnameText);
        exit(-1);
      }
    }
  }
  free(fnameText);
  fclose(tarfd2);
}





// The third one: for option -t: print the contents of the specified archive
void printArchive( char * tarfilename ) {
  // Int total for keeping track of the number of bytes read so that an end can be anticipated.
  int total;
  FILE *tarfd2;
  struct stat buf2;
  // Open the target tar file for reading.
  tarfd2 = fopen(tarfilename, "r");
  if (lstat(tarfilename, &buf2) < 0) {
    perror("lstat");
    exit(-1);
  }
  // Check if the specified target is a tar file
  int *magicChecker = (int *)malloc(sizeof(int));
  if ((int)fread(magicChecker, 1, sizeof(int), tarfd2) < sizeof(int)) {
      perror("fread");
      free(magicChecker);
      exit(-1);
  }
  if (magicChecker[0] != 0x7261746D) {
    fprintf(stderr, "Bad magic number\n");
    free(magicChecker);
    exit(-1);
  }
  free(magicChecker);
  total = total + (int)sizeof(int);
  // Check Passed!
  char *fnameText = (char *)malloc(sizeof(char)*1000);

  while(total < (int)buf2.st_size) {
    // Get the inode number
    long long int *inodeGet = (long long int *)malloc(sizeof(long long int));
    if ( (int)fread(inodeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
      perror("fread");
      free(inodeGet);
      free(fnameText);
      exit(-1);
    }
    int inode = inodeGet[0];
    free(inodeGet);
    total = total + (int)sizeof(long long int);

    if( !get_inode((ino_t)inode) ) {
      // Get the file name length
      int *nlenGet = (int *)malloc(sizeof(int));
      if ( (int)fread(nlenGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(fnameText);
        free(nlenGet);
        exit(-1);
      }
      int nameLen = nlenGet[0];
      free(nlenGet);
      total = total + (int)sizeof(int);
      // Get the file name
      if ( (int)fread(fnameText, 1, nameLen, tarfd2) < nameLen ) {
        perror("fread");
        free(fnameText);
        exit(-1);
      }
      total = total + nameLen;
      // Get the file mode
      int *modeGet = (int *)malloc(sizeof(int));
      if ( (int)fread(modeGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(fnameText);
        free(modeGet);
        exit(-1);
      }
      int fmode = modeGet[0];
      free(modeGet);
      total = total + (int)sizeof(int);
      set_inode( (ino_t)inode, fnameText );
      // Directories
      if (S_ISDIR((mode_t)fmode)) {
        // Get the file modification time
        long long int *timeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(timeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(timeGet);
          exit(-1);
        }
        int mtime = timeGet[0];
        free(timeGet);
        total = total + (int)sizeof(long long int);
        fnameText[nameLen] = '/';
        fnameText[nameLen + 1] = '\0';
        printf("%s -- inode: %llu, mode: %o, mtime: %llu\n", fnameText, (long long int)inode, fmode & 0x1ff, (long long int)mtime);
        // Soft links (possibly not exist)
      } else if (S_ISLNK((mode_t)fmode)) {
        // Do Nothing
        // Executable files
      } else if ((mode_t)fmode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        // Get the file modification time
        long long int *timeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(timeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(timeGet);
          exit(-1);
        }
        int mtime = timeGet[0];
        free(timeGet);
        total = total + (int)sizeof(long long int);
        // Get the file size
        long long int *sizeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(sizeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(sizeGet);
          exit(-1);
        }
        int fsize = sizeGet[0];
        free(sizeGet);
        total = total + (int)sizeof(long long int);
        if (fseek(tarfd2, fsize, SEEK_CUR) != 0) {
          perror("fseek");
          free(fnameText);
          exit(-1);
        }
        total = total + fsize;
        fnameText[nameLen] = '*';
        fnameText[nameLen + 1] = '\0';
        printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", fnameText, (long long int)inode, fmode & 0x1ff, (long long int)mtime, (long long int)fsize);
        // Regular Files
      } else {
        // Get the file modification time
        long long int *timeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(timeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(timeGet);
          exit(-1);
        }
        int mtime = timeGet[0];
        free(timeGet);
        total = total + (int)sizeof(long long int);
        // Get the file size
        long long int *sizeGet = (long long int *)malloc(sizeof(long long int));
        if ( (int)fread(sizeGet, 1, sizeof(long long int), tarfd2) < sizeof(long long int) ) {
          perror("fread");
          free(fnameText);
          free(sizeGet);
          exit(-1);
        }
        int fsize = sizeGet[0];
        free(sizeGet);
        total = total + (int)sizeof(long long int);
        if (fseek(tarfd2, fsize, SEEK_CUR) != 0) {
          perror("fseek");
          free(fnameText);
          exit(-1);
        }
        total = total + fsize;
        fnameText[nameLen] = '\0';
        printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", fnameText, (long long int)inode, fmode & 0x1ff, (long long int)mtime, (long long int)fsize);
      }
      // Hard Links
    } else {
      // Get the file name length
      int *nlenGet = (int *)malloc(sizeof(int));
      if ( (int)fread(nlenGet, 1, sizeof(int), tarfd2) < sizeof(int) ) {
        perror("fread");
        free(nlenGet);
        exit(-1);
      }
      int nameLen = nlenGet[0];
      free(nlenGet);
      total = total + (int)sizeof(int);
      // Get the file name
      char *fnameText = (char *)malloc(sizeof(char)*1000);
      if ( (int)fread(fnameText, 1, nameLen, tarfd2) < nameLen ) {
        perror("fread");
        free(fnameText);
        exit(-1);
      }
      total = total + nameLen;
      fnameText[nameLen] = '\0';
      printf("%s -- inode: %llu\n", fnameText, (long long int)inode);
    }
  }
  free(fnameText);
  fclose(tarfd2);
}
