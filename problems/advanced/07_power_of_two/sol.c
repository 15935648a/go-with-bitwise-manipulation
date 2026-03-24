#include <stdio.h>


int is_power_of_two( int a){
	return (a > 0) & !(a & (a - 1));
}


int main( void ) {
	int n;
	while (scanf("%d", &n) == 1) printf("%d\n", is_power_of_two(n));

	return 0;
}
