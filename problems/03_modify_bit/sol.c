#include <stdio.h>

int set_bit(int n, int k){
    int mask = 1 << k;
    return (n | mask);
}

int clear_bit(int n, int k){
    int mask = 1 << k;
    return (n & ~(mask));
}

int toggle_bit(int n, int k){
    int mask = 1 << k;
    return (n ^ mask);
}

int (*ops[3])(int, int) = {set_bit, clear_bit, toggle_bit};

int main ( void ){
    int n, k;
    char op;
    while (scanf(" %c %d %d", &op, &n, &k) == 3){
       int idx = (op == 'S') ? 0 : (op == 'C')? 1 : 2;
       int result = ops[idx](n , k);
        printf("%d\n", result);
    }
}
