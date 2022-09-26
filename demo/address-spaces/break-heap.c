#include <stdlib.h>
#include <stdio.h>

int main()
{
  int i;
  char *c;

  for (i = 0; i < 1000000000; i++) {
      //allocate 512MiB at a time
    c = (char *) malloc(1024*1024*512);
    printf("%9d %p\n", i, c);
    if (c == NULL) exit(0);
  }
  return 0;
}
