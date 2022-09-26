#include <stdio.h>

int main()
{
  char *s=0;
  char c;

    printf("c is stored at: %p. s is stored at %p and points to %p\n", &c, &s, s);
    c = 'a';
    *s = c;

    return 0;
}
