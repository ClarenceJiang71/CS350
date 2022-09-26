//
//  my_malloc-driver.c
//  Lab1: Malloc
//


#include "my_malloc.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

void test1();

int main(int argc, const char * argv[])
{
    test1();

    return 0;
}

void printFreeList(){
    FreeListNode ptr = free_list_begin();
    printf("First node at: %p\n", ptr);
    printf("Size of free chunk: %d\n", ptr->size);
    ptr=ptr->flink;

    while(ptr != NULL){
        printf("Next node at: %p\n", ptr);
        printf("Size of free chunk: %d\n", ptr->size);
        ptr = ptr->flink;
    }
}

void test1(){
    char *a = my_malloc(32);
    char *b = my_malloc(32);
    char *c = my_malloc(32);
    char *d = my_malloc(32);
    char *e = malloc(0);


    my_free(c);
    my_free(d);

    printFreeList();

    coalesce_free_list();

    printf("\nAfter coalesce:\n");
    printFreeList();
}


