#ifndef BITBOARD_H
#define BITBOARD_H

typedef unsigned long long u64;

extern u64 zobrist_table[361][2];

typedef struct {
	u64 black[6];
	u64 white[6];
	u64 hash;
} Board;

static const u64 NOT_LEFT_EDGE[6] = {
	~((1ULL << 0) | (1ULL << 19) | (1ULL << 38) | (1ULL << 57)),
	~((1ULL << 12) | (1ULL << 31) | (1ULL << 50)),
	~((1ULL << 5) | (1ULL << 24) | (1ULL << 43) | (1ULL << 62)),
	~((1ULL << 17) | (1ULL << 36) | (1ULL << 55)),
	~((1ULL << 10) | (1ULL << 29) | (1ULL << 48)),
	~((1ULL << 3) | (1ULL << 22) | (1ULL << 41) | (1ULL << 60))
};


static const u64 NOT_RIGHT_EDGE[6] = {
	~((1ULL << 18) | (1ULL << 37) | (1ULL << 56)),
	~((1ULL << 11) | (1ULL << 30) | (1ULL << 49)),
	~((1ULL << 4)  | (1ULL << 23) | (1ULL << 42) | (1ULL << 61)),
	~((1ULL << 16) | (1ULL << 35) | (1ULL << 54)),
	~((1ULL << 9)  | (1ULL << 28) | (1ULL << 47)),
	~((1ULL << 2)  | (1ULL << 21) | (1ULL << 40))
};

void set_stone(Board *b, int row, int col, int color);   // color: 1=black, 2=white
void clear_stone(Board *b, int row, int col, int color);
int  get_stone(Board *b, int row, int col);              // 回傳 0=空, 1=black, 2=white
void get_neighbors(u64 board[6], u64 result[6]);
/*color board是該玩家的面板, start是只有一個子的board, result 是整個連通的盤面*/
void flood_fill(u64 color_board[6], u64 start[6], u64 result[6]);
int count_liberties(Board *b, u64 group[6]);
void capture_group(Board *b, u64 group[6], int color);
int is_legal_move(Board *b, int row, int col, int color, u64 prev_hash);
void init_zobrist( void );
void print_board(Board *b);
#endif
