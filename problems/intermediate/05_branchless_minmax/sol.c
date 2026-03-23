#include <stdio.h>

/* if (condition) result = A
   Else result = B
   Can be done by if condition is either 1 or 0
   int mask = -(condition)
   return (result_A & mask) | (result_B & ~mask)*/


int bit_minmax(int a, int b,int is_max){
	int sign_a = (unsigned)a >> 31;
	int sign_diff = sign_a ^ ((unsigned)b >> 31);
	int mode = -is_max;


	int sa_mask = -sign_a;
	int select_A = sa_mask ^ mode;
	int diff_sign_result = (a & select_A) | (b & ~select_A);

	int diff = a - b;
	int sign = diff >> 31;
	int same_sign_result = b + (diff & (sign ^ mode));

	int diff_mask = -sign_diff;
	return (diff_sign_result & diff_mask) | (same_sign_result & ~diff_mask);
}

int bit_min(int a, int b) {return bit_minmax(a, b, 0);}
int bit_max(int a, int b) {return bit_minmax(a, b, 1);}

int main ( void ){
	int a, b;
	while ((scanf("%d %d", &a, &b) == 2)){
		printf("%d %d\n", bit_min(a, b), bit_max(a, b));
	}
	return 0;
}
