#include <stdio.h>


int popcount(unsigned int n) {
  n = ((n & 0x55555555) + ((n >> 1) & 0x55555555));
  n = ((n & 0x33333333) + ((n >> 2) & 0x33333333));
  n = ((n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F));
  n = ((n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF));
  n = ((n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF));
  return (n & 0x3F);
}

int find_msb_pos(unsigned int n){
	unsigned m = n | (n >> 1);
	m |= m >> 2;
	m |= m >> 4;
	m |= m >> 8;
	m |= m >> 16;
	return n ? (popcount(m) - 1) : -1;
}

int find_msb_pos_builtin( unsigned int n){
	return n ? (31 - __builtin_clz(n)) : -1;
}

int main ( void ){
	unsigned int n = 0;
	while(scanf("%u", &n) == 1) printf("%d\n", find_msb_pos(n));

	return 0;
}
