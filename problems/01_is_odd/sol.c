#include <stdio.h>

int is_odd(int a){
    int mask = 1;
    return (mask & a);
}


int main( void ){
    int n;
    while(scanf("%d", &n) == 1){
        printf("%d\n", is_odd(n));
    }
    return 0;
}
