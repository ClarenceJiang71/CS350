#include<stdio.h>
#include<stdlib.h>

// Creates a 2x3 int array and initializes it with 0's
int **create(){
    int **x;
    x = malloc(2 * sizeof(int*));
    for (int i = 0; i < 2; i++)
        x[i] = malloc(3 * sizeof(int));
    
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 3; j++)
            x[i][j] = 0;
    }

    return x;
}

void fill(int **x){
    x[0][0] = 0; x[0][1] = 1; x[0][2] = 2;
    x[1][0] = 3; x[1][1] = 4; x[1][2] = 5;
}


void foo(int **add){
    free(add);
}

void delete(int **x){
    for(int i = 0; i < 2; i++){
        free(x[i]);
    }
    // free(x[0]);
    foo(x);
}

void print_array(int *a[]){
    printf("a:\t%2d, %2d, %2d\n", a[0][0], a[0][1], a[0][2]);
    printf("  \t%2d, %2d, %2d\n\n", a[1][0], a[1][1], a[1][2]);
}

int main(int argc, char *argv[]){
    int **a, **b, **c;
    int s[2][3];

    printf("a[0][0]: %d\n", a[0][0]);
    fill(b);
    a = create();
    b = create();
    c = create();
    print_array(a);
    fill(a);
    print_array(a);
    printf("a[1][3] = %d\n", a[0][3]);
    printf("s[1][3] = %d\n", s[0][3]);
    delete(a);
    delete(s);
    b = a;
    delete(b);
    delete(c);
    print_array(a);
    printf("a[0]: %p\n", a[0]);
    return 0;

}
