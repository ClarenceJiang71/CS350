//This demo is based on Prof. James Plank's CS360 at UTK

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef unsigned long int UI;

//externally defined variables
extern int etext; //end of code (text) segment
extern int edata; //end of initialized data segment
extern int end;  //end of uninitialized data segment

//Four global variables. 2 initialized, 2 uninitialized
int int_init1 = 4;
int int_uninit1;
int int_init2 = 6;
int int_uninit2;

void where_is_addr( void *addr, const char * s ) {
    void * text_end = &etext;
    void * init_end = &edata;
    void * uninit_end = &end;
    void * heap_end = sbrk(0);
    static int first_time=1;
    
    if( first_time ){
        printf("text_end: %p\n", text_end);
        printf("init_end: %p\n", init_end);
        printf("uninit_end: %p\n", uninit_end);
        printf("heap_end: %p\n\n", heap_end);
        first_time = 0;
    }
    
    if( addr < text_end ){
        printf("%s -> Text segment: **%p** < text_end(%p) )\n", s, addr, text_end );
    }
    else if ( addr < init_end ){
        printf("%s -> Initialized Data segment: text_end(%p) < **%p** < init_end(%p)\n", s, text_end, addr, init_end );
    }
    else if ( addr < uninit_end ){
        printf("%s -> Unnitialized Data segment: init_end(%p) < **%p** < unit_end(%p)\n", s, init_end, addr, uninit_end );
    }
    else if ( addr < heap_end ){
        printf("%s -> Heap segment: uninit_end(%p) < **%p** < heap_end(%p)\n", s, uninit_end, addr, heap_end );
    }
    else {
        printf("%s -> Stack? **%p** > heap_end(%p)\n", s, addr, heap_end);
    }
}

/* foo and main should point to code (text) segment. */

void func_foo(int ii)
{
    static int called=0; //static, initialized? var
    int i;
    
    if( called == 0 ){
        where_is_addr(&called, "func_foo 1st: &called");
        where_is_addr(&i, "func_foo 1st: &i");
        where_is_addr(&ii, "func_foo 1st: &ii");
        called++;
        func_foo(1);
    }
    else{
        where_is_addr(&called, "func_foo 2nd: &called");
        where_is_addr(&i, "func_foo 2nd: &i");
        where_is_addr(&ii, "func_foo 2nd: &ii");
    }
}

/* And of course i and buf will belong
   on the stack.  I will call malloc()
   to set buf equal to an address in
   the heap. */

int main(int argc, char **argv)
{
    int i;  //stack variable
    char *buf; //stack variable

    //buf should point to dynamically allocated heap memory
    buf = (char *) malloc(200);


    printf("Code Addresses:\n");
    where_is_addr( main, "main()" );
    where_is_addr( func_foo, "func_foo()" );


    printf("\nGlobal Variable Addresses:\n");
    where_is_addr( &int_init1, "&int_init1" );
    where_is_addr( &int_init2, "&int_init2" );
    where_is_addr( &int_uninit1, "&int_uninit1" );
    where_is_addr( &int_uninit2, "&int_uninit2" );
    

    printf("\nHeap Address:\n");
    where_is_addr( buf, "buf" );


    printf("\nStack Addresses:\n");
    where_is_addr( &i, "&i" );
    where_is_addr( &buf, "&buf" );
    where_is_addr( &argc, "&argc" );

    printf("\n");
    func_foo(0);
    
  /* Finally, print the addressses as
     recorded in the directory "/proc". */
    printf("\n\nPrinting /proc/%d/maps:\n", getpid());
    sprintf(buf, "cat /proc/%d/maps | grep -E 'heap|stack' ", getpid());
    system(buf);
    
    return 0;
}
