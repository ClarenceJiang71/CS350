#include<stdio.h>
#include<stdlib.h>

struct time {
    int hr;
    int min;
    int sec;
};

typedef struct time time;
typedef int size;

void change_struct(struct time *a){
    a->hr = 20;
    return;
}

int main(int argc, char const *argv[])
{
    time *a;
    time b = {9, 30, 1};

    size x = 4;
    printf("%d\n", x);

    change_struct(&b);

    a = &b;

    printf("Time: %d:%02d:%02d\n", a->hr, a->min, a->sec);

    a = (struct time*)malloc(8*sizeof(struct time));

    printf("Time: %d:%02d:%02d\n", a->hr, a->min, a->sec);

    free(a);

    return 0;
}
