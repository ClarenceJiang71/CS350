#include <stdio.h>

int main(){
    int x[] = {1,2,3,4};

    printf("The address of x[0] is: %p\n", &x[0]);
    printf("The address of x is %p\n", &x);

    // printf("The value of x[4] is %d\n", x[4]); 
}