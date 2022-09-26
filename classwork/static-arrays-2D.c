#include <stdio.h>

int main(int argc, char *argv[] ){
    char carray2[2][3]={'f','o','o','b','a','r'};
    // char carray2[2][3]={{'f','o','o'},{'b','a','r'}};


    printf("\nsizeof(carray2): %lu. addr of carray2: %p\n", sizeof(carray2), carray2 );
    for(int i=0; i<2; i++) {
        printf("carray2[%d]: (%p)\n", i, carray2[i] );
        for( int j=0; j<3; j++){
            printf("\tcarray2[%d][%d]: '%c' (%p)\n", i, j, carray2[i][j], &carray2[i][j]);
        }
    }
    
    return 0;
}
