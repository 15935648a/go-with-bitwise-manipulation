#include <stdio.h>

int popcount(unsigned int n) {
  n = ((n & 0x55555555) + ((n >> 1) & 0x55555555));
  n = ((n & 0x33333333) + ((n >> 2) & 0x33333333));
  n = ((n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F));
  n = ((n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF));
  n = ((n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF));
  return (n & 0x3F);
}

int main(void) {
  unsigned int n;
  while (scanf("%u", &n) == 1) {
    printf("%d\n", popcount(n));
  }
  return 0;
}
