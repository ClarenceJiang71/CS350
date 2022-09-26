#include <stdio.h>

int main( int argc, char *argv[] ){
    int i;
    int * ip;
    
    printf("&i: %p. i: %d\n", &i, i);
    printf("&ip: %p. ip: %p.\n\n", &ip, ip);

    
    printf("Setting i's value to 5. Press return to continue ...");
    getchar(); //reads but discards a char from the terminal
    i=5;
    printf("\n&i: %p. i: %d\n", &i, i);
    printf("&ip: %p. ip: %p.\n\n", &ip, ip);

    
    printf("Pointing ip to i. Press return to continue ...");
    getchar(); //reads but discards a char from the terminal
    ip=&i;
    printf("\n&i: %p. i: %d\n", &i, i);
    printf("&ip: %p. ip: %p. *ip: %d\n\n", &ip, ip, *ip);
    
    
    printf("Setting *ip to 123. Press return to continue ...");
    getchar(); //reads but discards a char from the terminal
    *ip=123;
    printf("\n&i: %p. i: %d\n", &i, i);
    printf("&ip: %p. ip: %p. *ip: %d\n\n", &ip, ip, *ip);

    return 0;
}
