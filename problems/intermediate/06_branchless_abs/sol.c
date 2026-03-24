#include <stdio.h>


int branchless_abs(int a){
/* when a is positive, sign = 0x00000000, a ^ sign = a, a - 0 = a
   when a is negative, sign = 0xFFFFFFFF, a ^ sign = a's one's complement, when a's one's complement + 1, it would be -a*/

	int sign = a >> 31;
	return (a ^ sign) - sign;
}


int main ( void ){
	int n;
	while (scanf("%d", &n) == 1) printf("%d\n", branchless_abs(n));

	return 0;
}
