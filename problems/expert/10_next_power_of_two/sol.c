#include <stdio.h>

unsigned int next_power_of_two( unsigned int n){
	--n; /* 處理n本來就是2的次方*/
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >>16;

	return (++n);
}


int main ( void ){
	unsigned int n = 0;
	while (scanf("%u", &n) == 1) printf("%u\n", next_power_of_two(n));

	return 0;
}
