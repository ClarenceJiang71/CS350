#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
    int *ip = malloc(10*sizeof(int));
    long l = (long)ip;
    printf("Value of l: %#lx\n", l);

    double d = 3.5;
    l = d;

    printf("Value of d: %lf\n", d);
    printf("Value of l: %ld\n", l);

    char **chararray2D;
    char *chararray;

    chararray = (char*)chararray2D;

    return 0;
}