#include<stdio.h>
#include<unistd.h>

typedef struct{
    char c;  // 1 byte + 7 padding
    char* cp; // 8 bytes
    int i;  // 4 bytes + 4 padding
}My_Type; // 24 total bytes

void increment(){
    static My_Type **ptr = NULL;
    static int init = 0;

    if (!init){ 
        ptr = sbrk(0);
        printf("Value of ptr: %p\n", ptr);
        init++;
        return;
    }

    ptr++;

    printf("Value of ptr: %p\n", ptr);

}

int main(int argc, char *argv[]){
    printf("sizeof(My_Type) = %lu\n", sizeof(My_Type));
    for(int i = 0; i < 10; i++){
        increment();
    }
}

