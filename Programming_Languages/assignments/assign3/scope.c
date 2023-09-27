#include <stdio.h>

static void f(int *a, int *b, int *c){
    *a += 5;
    *b += 5;
    *c += 5;
    printf("f: a=%d b=%d c=%d\n", *a, *b, *c);
}

static void g(int *a, int *b){
    int c = 40;
    *b += 5;
    c += 5;
    f(a, b, &c);
    printf("g: a=%d b=%d c=%d\n", *a, *b, c);
}

int main(int argc, char *argv[])
{
    int a = 10;
    int b = 20;
    int c = 30;
    g(&a, &b);
    printf("main: a=%d b=%d c=%d\n", a, b, c);
}