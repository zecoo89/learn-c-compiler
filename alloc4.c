#include<stdio.h>
#include<stdlib.h>

void alloc4(int **p, int a, int b, int c, int d) {
  //printf("*p_bef: %p\n", *p);
  *p = malloc(sizeof(int) * 4);
  //printf("*p_aft: %p\n", *p);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;

  //printf("p: %p\n", p);
  //printf("%p: %d, %p: %d, %p: %d, %p: %d\n", *p, (*p)[0], *p+1, (*p)[1], *p+2, (*p)[2], *p+3, (*p)[3]);
}

void print(int *p) {
  printf("print: %p\n", p);
}

void print_int(int a) {
  printf("int: %d\n", a);
}
