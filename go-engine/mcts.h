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


MCTSNode *mcts_new_node(Board *b, int row, int col, int color, MCTSNode *parent);
MCTSNode *mcts_select(MCTSNode *root);
void      mcts_expand(MCTSNode *node);
int       mcts_simulate(MCTSNode *node);
void      mcts_backprop(MCTSNode *node, int result);
MCTSNode*       mcts_best_move(Board *b, int color, int iterations, int *out_row, int *out_col);
void mcts_free(MCTSNode *node);
MCTSNode *mcts_advance(MCTSNode *root, int row, int col);



#endif // !MCTS.H
