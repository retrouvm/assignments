// gcc -Wall sum7.c -o sum7
// ./sum7 4 8 -5 0 20
// prints: Sum=27 Sum2=505 Sum3=8451

#include <stdio.h>
#include <stdlib.h>

#define sum7(argc, argv, x, y, z) \
  x = 0; \
  y = 0; \
  z = 0; \
  
  
int main(int argc, char *argv[]) {
  int x, y, z;
  sum7(argc, argv, x, y, z);
  printf("Sum=%d Sum2=%d Sum3=%d\n", x, y, z);
  return 0;
}
