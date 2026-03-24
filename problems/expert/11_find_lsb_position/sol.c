#include <stdio.h>


int popcount(unsigned int n) { // from 04;
  n = ((n & 0x55555555) + ((n >> 1) & 0x55555555));
  n = ((n & 0x33333333) + ((n >> 2) & 0x33333333));
  n = ((n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F));
  n = ((n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF));
  n = ((n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF));
  return (n & 0x3F);
}


int find_lsb_pos(unsigned int n){
	return n ? popcount((n & -n) - 1) : -1;
	/*用三元運算子避免n = 0 時依然進入計算*/
}

int find_lsb_pos_builtin(unsigned int n){
	return n ? __builtin_ctz(n) : -1;
	/*內建函式，不用重複造輪子*/
}


int main ( void ){
	unsigned int n = 0;
	while(scanf("%u", &n) == 1) printf("%d\n", find_lsb_pos(n));
	return 0;
}

