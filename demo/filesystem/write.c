#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd, sz, flags, len;
    char *outword;

    if (argc < 3) {
        fprintf(stderr, "usage: %s w|wc|wt|wct input-word\n", argv[0]);
        exit(1);
    }
    outword = argv[2];
    
    //Figure out "flags" for open()
    if (strcmp(argv[1], "w") == 0) {
      flags = O_WRONLY;
    } else if (strcmp(argv[1], "wc") == 0) {
      flags = (O_WRONLY | O_CREAT);
    } else if (strcmp(argv[1], "wt") == 0) {
      flags = (O_WRONLY | O_TRUNC);
    } else if (strcmp(argv[1], "wa") == 0) {
        flags = (O_WRONLY | O_APPEND);
    } else if (strcmp(argv[1], "wct") == 0) {
      flags = (O_WRONLY | O_CREAT | O_TRUNC);
    } else if (strcmp(argv[1], "wca") == 0) {
      flags = (O_WRONLY | O_CREAT | O_APPEND);
    } else {
        fprintf( stderr, "Bad argument: %s! Must be 'w', 'wc', 'wt', 'wa', 'wct', or 'wca'.\n", argv[1] );
        exit(1);
    }

    /* Open the file with the given flags. */

    fd = open("write2-out.txt", flags, 0644);
    if (fd < 0){
        perror("open()"); exit(1);
    }
    else{
        printf("open() returned %d\n\n", fd);
    }

    len = strlen(outword);
    sz = write(fd, outword, len);
    
    printf("write(%d, \"%s\", %d) returned %d\n", fd, outword, len, sz);

    close(fd);
    exit(0);
}
