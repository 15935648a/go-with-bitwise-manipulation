#include <stdio.h>
#include <stdlib.h>

int find_unique(int* array, int n){
	int u = 0;
	for (int i = 0; i < n; i++){
		u = u ^ array[i]; // 累加XOR,找到唯一值
	}
	return u;
}

int main ( void ){	
	int n = 0;
	while(scanf("%d", &n) == 1){
		int* array = malloc(n * sizeof(int)); //別忘了free!!
		for (int i = 0; i < n; i++){
			scanf("%d ", &array[i]);
		}
		printf("%d\n", find_unique(array, n));
		free(array);
	}
}
