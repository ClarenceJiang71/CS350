#include<stdio.h>
#include"my_header.h"

#define BUFFER_SIZE 30

int main(int argc, char *argv[]){
    int y = 10;
    y = square(y);

    printf("The square of 10 is %d]\n", y);

    int b[BUFFER_SIZE];
    printf("%p\n", b);
    test(b);

    #ifndef CHECK
        fprintf(stderr, "Check was defined!\n");
    #endif
}


int test(int a[BUFFER_SIZE]){

    printf("%p\n", a);
    printf("%d\n", a[15]);
    return 0;
}
