#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ALLOCSIZE 24*1024
#define NITERATIONS 1024

void print_heap() {
    static char *oldheapptr=NULL, *curheapptr=NULL;
    
    if( oldheapptr == NULL ){
        curheapptr = oldheapptr = sbrk(0);
        printf("Original heap end: %p\n", curheapptr );
        return;
    }
    
    curheapptr = sbrk(0);
    if ( oldheapptr != curheapptr ){
        printf("\tHeap moved %ld bytes, from %p to %p\n",
               (unsigned long)curheapptr - (unsigned long)oldheapptr, oldheapptr, curheapptr );
        oldheapptr = curheapptr;
    }
}

int main() {
    char * ptr;
    
    print_heap();
    
    for(int i=0; i<NITERATIONS; i++ ) {
        ptr = (char*)malloc(ALLOCSIZE);
        printf("%d: malloc(%d)\n", i, ALLOCSIZE);
        print_heap();
    }
    return 0;
}
