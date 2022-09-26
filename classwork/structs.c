#include <stdio.h>

struct charintchar {
    char c1;
    int i;
    char c2;
};

typedef struct charcharint{
    char c1;
    char c2;
    int i;
} CCI;

int main( int argc, char *argv[] ){
    struct charintchar cic = {'a', 10, 'c'};
    CCI cci = {'b', 'd', 9};
    CCI * ccip = &cci;

    printf("Struct values:\n");
    printf("\tcic: %c %c %d (size: %lu)\n", cic.c1, cic.c2, cic.i, sizeof(cic) );
    printf("\tcci: %c %c %d (size: %lu)\n", ccip->c1, ccip->c2, ccip->i, sizeof(cci) );

    //return 0;
    
    printf("\nStruct memory addresses:\n");
    printf("\t&cic: %p\n\t&cic.c1: %p\n\t&cic.i: %p\n\t&cic.c2: %p\n\n", &cic, &(cic.c1), &(cic.i), &(cic).c2 );
    printf("\t&cci: %p\n\t&cci.c1: %p\n\t&cci.c2: %p\n\t&cci.i: %p\n", &cci, &(cci.c1), &(cci).c2, &(cci.i) );
}
