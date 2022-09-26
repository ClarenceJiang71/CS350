#include<stdio.h>

int print_array(int *x){
    for (int i = 0; i < 10; i++){
        printf("x[%d] = %d\n", i, x[i]);
    }
}



int main(int argc, char *argv[])
{
    /* code */
    int a[10] = {1,2,3};
    print_array(a);
}
