#include <stdio.h>

void print_chararray( char c[], unsigned long len ){
    printf("Array address: %p; len: %lu; sizeof(c):%lu\n", c, len, sizeof(c) );
    
    for( int i=0; i<len; i++){
        printf("\tarray[%2d]: '%c' (%d)\n", i, c[i], c[i]);
    }
    printf("\tArray as string: \"%s\"\n\n", c );
}

int main(int argc, char *argv[] ){
    char carray1[]="Carray1";
    char carray2[]={'C', 'a', 'r', 'r', 'a', 'y', '2'};
    char carray3[100];
    
    carray3[0] = 'C';
    carray3[1] = 'a';
    carray3[2] = 'r';
    carray3[3] = 'r';
    carray3[4] = 'a';
    carray3[5] = 'y';
    carray3[6] = '3';
    carray3[7] = '\0';
    
    printf("carray1: %p, len: %lu\n", carray1, sizeof(carray1));
    print_chararray(carray1, sizeof(carray1));
    
    printf("carray2: %p, len: %lu\n", carray2, sizeof(carray2));
    print_chararray(carray2, sizeof(carray2));
    
    printf("carray3: %p, len: %lu\n", carray3, sizeof(carray3));
    print_chararray(carray3, sizeof(carray3));

    return 0;
}
