#include "bitboard.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
u64 zobrist_table[361][2];

void set_stone(Board *b, int row, int col, int color) {
	int pos = row * 19 + col; /*共有19個row,也就是說第(i,j)的子會是i * 19 +
	j的位置*/
	int idx = pos >> 6;   //  這裡等價除以64，所以可以找到選哪個 uint64(b[0]-b[64])
	int bit = pos & 63;   // 這裡是找餘數，等於在找選哪個 bit

	if (color == 1)
        	b->black[idx] |= (1ULL << bit);
	else if (color == 2)
		b->white[idx] |= (1ULL << bit);
	b->hash ^= zobrist_table[pos][color - 1];
}

void clear_stone(Board *b, int row, int col, int color) {
	int pos = row * 19 + col;
	int idx = pos >> 6;
	int bit = pos & 63;

	if (color == 1)
		b->black[idx] &= ~(1ULL << bit); //把那個bit設為0;
	else if (color == 2)
		b->white[idx] &= ~(1ULL << bit);	
	b->hash ^= zobrist_table[pos][color - 1];
}

int get_stone(Board *b, int row, int col) {
	int pos = row * 19 + col;
	int idx = pos >> 6;
	int bit = pos & 63;

	if ((b->black[idx] >> bit) & 1) return 1; //把目標 bit 移到最低位，& 1 取出它的值（0 或 1）;
	if ((b->white[idx] >> bit) & 1) return 2;

	return 0;
}

void get_neighbors(u64 *board,u64 *result){
	//找右邊的子
	for (int i = 0; i < 6; i++)
		result[i] |= (board[i] & NOT_RIGHT_EDGE[i]) << 1;
	//找左邊的子
	for (int i = 0; i < 6; i++)
		result[i] |= (board[i] & NOT_LEFT_EDGE[i]) >> 1;
	//找上面的子
	for (int i = 5; i > 0; i--)
		result[i] |= (board[i] << 19) | (board[i-1] >> (64 - 19));
	result[0] |= board[0] << 19;
	//找下面的子
	for (int i = 0; i < 5; i++){
		result[i] |= (board[i] >> 19) | (board[i+1] << (64 - 19));
	}

	result[5] &= (1ULL << 41) - 1; //只保留bit 0-40
}

void flood_fill(u64 *color, u64 *start, u64 *result){
	u64 prev[6];
	for (int i = 0; i < 6; i++){
		result[i] = start[i];
	}
	
	do{
		for (int i = 0; i < 6; i++){
			prev[i] = result[i];
		}
		u64 neighbors[6] = {0};
		get_neighbors(result, neighbors);
		for (int i = 0; i < 6; i++){
			result[i] |= neighbors[i] & color[i];
		}
	} while(memcmp(prev,result,sizeof(prev)) !=0);


}

int count_liberties(Board *b, u64 group[6]){
	u64 empty[6];
	u64 neighbors[6] = {0};
	u64 liberties[6];
	for (int i = 0; i < 6; i++){
		empty[i] = ~(b->black[i] | b->white[i]);
	}
	get_neighbors(group, neighbors);
	for (int i = 0; i < 6; i++){
		liberties[i] = neighbors[i] & empty[i];
	}
	int total = 0;
	for (int i = 0; i < 6; i++){
		total +=__builtin_popcountll(liberties[i]);
	}
	return total;
}

void capture_group(Board *b, u64 group[6], int color){
	for (int i = 0; i < 6; i++){
		u64 stones = (color == 1) ? b->black[i] : b->white[i];
		u64 captured = stones & group[i];  // 實際被提掉的棋子
		
		// 對每顆被提掉的棋子更新 has
		u64 tmp = captured;
		while (tmp) {
			int bit = __builtin_ctzll(tmp);
			int pos = i * 64 + bit;
			b->hash ^= zobrist_table[pos][color - 1];
			tmp &= tmp - 1;  // 清掉最低位的 1
		}
		if (color == 1){
			b->black[i] &= ~group[i];
		} else {
			b->white[i] &= ~group[i];
		}
	}

}

void init_zobrist(void) {
	srand(12345);
	for (int i = 0; i < 361; i++){
		for (int j = 0; j < 2; j++){
			zobrist_table[i][j] = ((u64)rand() << 32) | rand();
		}
	}
}

int is_legal_move(Board *b, int row, int col, int color, u64 prev_hash){
	// 1. 空格檢查
	if (get_stone(b, row, col) != 0) return 0;
	
	// 2. 試落子
	set_stone(b, row, col, color);
	
	// 找自己這塊的連通群
	int pos = row * 19 + col;
	u64 start[6] = {0};
	start[pos >> 6] |= (1ULL << (pos & 63));

	u64 *my_board = (color == 1) ? b->black : b->white;
	u64 group[6] = {0};
	flood_fill(my_board, start, group);
	
	// 3. 自殺手：落子後氣為 0
	int libs = count_liberties(b, group);
	
	// 4. Ko 檢查
	int ko = (b->hash == prev_hash);

	clear_stone(b, row, col, color);

	if (libs == 0) return 0;  // 自殺手
	if (ko)        return 0;  // Ko
	
	return 1;
}

void print_board(Board *b){
	for (int r = 0; r < 19; r++){
		for (int c = 0; c < 19; c++){
			int s = get_stone(b, r, c);
			if (s == 1){
				printf("X ");
			} else if (s == 2){
				printf("O ");
			} else {
				printf(". ");
			}
		}
		printf("\n");
	}
	printf("\n");
}
