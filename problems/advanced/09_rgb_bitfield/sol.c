#include <stdio.h>

int get_r(unsigned int color){
	int mask = 0xFF;
	return (mask) & (color >> 16); // R會放在bit 23-16
}

int get_g(unsigned int color){
	int mask = 0xFF;
	return (mask) & (color >> 8);// G會放在bit 15-8
}

int get_b(unsigned int color){
	int mask = 0xFF;
	return (mask) & (color);//B會放在bit 0-7
}

unsigned int pack_rgb(int r, int g, int b){
	return ((unsigned int)r << 16 |(unsigned int)g << 8 |(unsigned int)b);//記得要轉換成unsigned
}

int main ( void ){
	unsigned int color;
	while (scanf("%u", &color) == 1){
		printf("%d %d %d\n", get_r(color), get_g(color), get_b(color));
	}
	return 0;
}


