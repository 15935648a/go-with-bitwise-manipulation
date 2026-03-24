#ifndef MCTS_H
#define MCTS_H
#include "bitboard.h"


typedef struct MCTSNode{
	Board board;
	int move_row;
	int move_col;
	int color;
	int win;
	int visit;
	int rave_win[361];
	int rave_visit[361];
	struct MCTSNode *parent;
	struct MCTSNode **children;
	int num_children;
} MCTSNode;


extern int pat_win[1 << 16];
extern int pat_visit[1 << 16];

MCTSNode *mcts_new_node(Board *b, int row, int col, int color, MCTSNode *parent);
MCTSNode *mcts_select(MCTSNode *root);
void      mcts_expand(MCTSNode *node);
int       mcts_simulate(MCTSNode *node, int *moves_played, int *num_moves);
void      mcts_backprop(MCTSNode *node, int result, int *moves_played, int num_moves);
MCTSNode* mcts_best_move(Board *b, int color, double time_limit_ms, int *out_row, int *out_col);
void      mcts_free(MCTSNode *node);
MCTSNode *mcts_advance(MCTSNode *root, int row, int col);

void train_one_game(void);
int  bench_one_game(int black_policy, int white_policy, double mcts_time_ms);
void save_pattern_table(const char *filename);
int  load_pattern_table(const char *filename);



#endif // !MCTS.H
