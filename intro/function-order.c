#include<stdio.h>

int square(int);

int main(){
    int x;
    printf("Enter a value: ");
    scanf("%d", &x);
    int y = square(x);
    printf("The square of the value is: %d\n", y);
}

int square(int x){
    return x*x;
}