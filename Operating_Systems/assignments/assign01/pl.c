#include <stdio.h>

#define N 1000

typedef struct {
	char x;
	long y;
	char z;
} s1_t;

typedef struct {
	char x;
	int y;
	char z;
} s2_t;

typedef struct {
	int vals[N];
} s3_t;

static void func1(s3_t param) {
	param.vals[0] = 200;
}

static void func2(s3_t *param) {
	param->vals[0] = 200;
}

int main(int argc, char *arv[]) {
	s3_t s3;
	s3_t s4;
	long long k;

	printf("Size(char) = %ld\n", sizeof(char));
	printf("Size(short) = %ld\n", sizeof(short));
	printf("Size(int) = %ld\n", sizeof(int));
	printf("Size(long) = %ld\n", sizeof(long));
	printf("Size(long long) = %ld\n", sizeof(k));
	printf("Size(void *) = %ld\n", sizeof(void *));

	printf("Size(s1_t) = %ld\n", sizeof(s1_t));
	printf("Size(s2_t) = %ld\n", sizeof(s2_t));

	s3.vals[0] = 100;
	func1(s3);
	printf("s3.vals[0] = %d\n", s3.vals[0]);

	s4.vals[0] = 100;
	func2(&s4);
	printf("s4.vals[0] = %d\n", s4.vals[0]);

	return 0;
}