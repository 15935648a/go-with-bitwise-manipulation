#include <stdio.h>

int get_bit(int n, int k){
    return ((n >> k) & 1);
}

int main ( void ){
    int n, k;
    while(scanf("%d %d", &n, &k) == 2){
        printf("%d\n", get_bit(n,k));
    }
    return 0;
}
