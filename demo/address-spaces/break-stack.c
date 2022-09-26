#include <stdio.h>

int main(int argc, char * argv[])
{
  char eight_K[8192];

  printf("Argc = %5d.  &argc = %p.\n", argc, &argc);
  main(argc+1, NULL);
  return 0;
}
