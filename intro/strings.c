#include<stdio.h>

int main(){
    char d[] = "foo";

    d[2] = '\0';
    printf("%s\n", d);
    printf("Sizeof(d) = %lu\n", sizeof(d));
}