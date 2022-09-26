#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
    int i = 5;
    long l;

    l = (long)&i;
    printf("The value of l: %#lx\n", l);
    printf("The value of &i: %p\n", &i);


    float f = 3.5;
    int j = f;
    printf("The value of f: %.2f\n", f);
    printf("The value of j: %d\n", j);

    char **array_2d = NULL;
    int *array_1d;

    array_1d = malloc(8*sizeof(int));

    void *ptr = main;
    // *(char*)ptr = 0xF;
    printf("The value of *ptr is: %#x\n", *(int*)ptr);


    ptr = sbrk(0);
    char *bad = ptr-4;
    for (int i = 0; i < 4; i++){
        bad[i] = i;
    }

    for(int i=0; i<4; i++)
        printf("%d\n", bad[i]);

    printf("The value of bad: %c\n", *bad);

    int *ip = malloc(10*sizeof(int));
    for(int k = 0; k < 10; k++)
        ip[k] = k;
    
    printf("ip: { ");
    for(int k = 0; k < 10; k++)
        printf("%d, ", ip[k]);
    printf("}\n");

    free(ip);

    printf("ip: { ");
    for(int k = 0; k < 10; k++)
        printf("%d, ", ip[k]);
    printf("}\n");

    // Weird Stuff
    for(int k = 0; k < 10; k++)
        ip[k] = k;
    printf("ip: { ");

    for(int k = 0; k < 10; k++)
        printf("%d, ", ip[k]);
    printf("}\n");

    // ip = malloc(11*sizeof(int));
    // free(ip);

    // printf("Address in ip: %p\n", ip);
    // printf("End of heap: %p\n", sbrk(0));
    // ip = malloc(9*sizeof(int));
    // printf("Address in ip: %p\n", ip);


    return 0;
}
