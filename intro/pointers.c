#include<stdio.h>

int main(){
    char *p;
    char c = 'A';
    p = &c;

    printf("Value of the pointer is %p\n", p);
    printf("Value pointed at by p is %c\n", *p);
    printf("Value of c is: %c\n", c);
    printf("The address of c is %p\n\n", &c);
    printf("The address of p is %p\n", &p);
}