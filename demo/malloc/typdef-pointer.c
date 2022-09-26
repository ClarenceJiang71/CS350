#include<stdio.h>

typedef int* IntPointer;
typedef int Length;

int main(int argc, char *argv[]){
    IntPointer i1;
    IntPointer *i3; //2D array of ints
    Length i2;

    printf("Size of i1: %lu\n", sizeof(i1));
    printf("Size of i2: %lu\n", sizeof(i2));
    return 0;   
}