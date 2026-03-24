#include "mcts.h"
#include "bitboard.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void mcts_free(MCTSNode *node) {
	if (!node) return;
	for (int i = 0; i < node->num_children; i++)
		mcts_free(node->children[i]);
	free(node->children);
	free(node);
}

MCTSNode *mcts_advance(MCTSNode *root, int row, int col) {
	for (int i = 0; i < root->num_children; i++) {
		MCTSNode *child = root->children[i];
		if (child->move_row == row && child->move_col == col) {
			root->children[i] = NULL;
			child->parent = NULL;
			mcts_free(root);
			return child;
		}
	}
	mcts_free(root);
	return NULL;
}


static int move_score(Board *b, int r, int c, int color) {
	int score = 0;
	int opponent = (color == 1) ? 2 : 1;

	Board tmp = *b;
	set_stone(&tmp, r, c, color);

	int pos = r * 19 + c;
	u64 stone[6] = {0};
	stone[pos >> 6] |= (1ULL << (pos & 63));
	u64 adj[6] = {0};
	get_neighbors(stone, adj);

	u64 *opp_board = (opponent == 1) ? tmp.black : tmp.white;

	for (int i = 0; i < 6; i++){
		u64 opp = adj[i] & opp_board[i];
		while (opp) {
			int bit = __builtin_ctzll(opp);
			int p = i * 64 + bit;
			u64 start[6] = {0};
			start[p >> 6] |= (1ULL << (p & 63));
			u64 group[6] = {0};
			flood_fill(opp_board, start, group);
			if (count_liberties(&tmp, group) == 0) score += 10;
			opp &= opp - 1;
		}
	}
	u64 *my_board = (color == 1) ? tmp.black : tmp.white;
	u64 start[6] = {0};
	start[pos >> 6] |= (1ULL << (pos & 63));
	u64 group[6] = {0};
	flood_fill(my_board, start, group);
	int libs = count_liberties(&tmp, group);
	if (libs >= 3) score += 3;
	else if (libs == 2) score += 1;

	return score;
}

MCTSNode *mcts_new_node(Board *b, int row, int col, int color, MCTSNode *parent){
	MCTSNode *node = malloc(sizeof(MCTSNode));
	node->board      = *b;
	node->move_row   = row;
	node->move_col   = col;
	node->color      = color;
	node->win        = 0;
	node->visit      = 0;
	node->parent     = parent;
	node->children   = NULL;
	node->num_children = 0;
	return node;
}

static double ucb(MCTSNode *node){
	if(node->visit == 0) return 1e9;
	return (double)node->win / node->visit
	+ 1.41 * sqrt(log(node->parent->visit) / node->visit); 
}

MCTSNode *mcts_select(MCTSNode *node){
	while (node->num_children > 0){
		MCTSNode* best = node->children[0];
		double best_score = ucb(best);
		for (int i = 1; i < node->num_children; i++){
			double score = ucb(node->children[i]);
			if (score > best_score || (score == best_score && rand() % 2)){
				best_score = score;
				best = node->children[i];
			}
		}
		node = best;
	}
	return node;
}

void mcts_expand(MCTSNode *node){
	int next_color = (node->color == 1) ? 2 : 1;
	node->children = malloc(361 * sizeof(MCTSNode *));
	
	u64 prev_hash = node->parent ? node->parent->board.hash : 0;

	for (int r = 0; r < 19; r++){
		for (int c = 0; c < 19; c++){
			if (is_legal_move(&node->board, r, c,next_color, prev_hash)){
				Board next = node->board;
				set_stone(&next, r, c, next_color);
				check_captures(&next, r, c, (next_color == 1) ? 2 : 1);
				node->children[node->num_children++] = 
      					mcts_new_node(&next, r, c, next_color, node);
			}
		}
	}
}

int mcts_simulate(MCTSNode *node){
	Board b = node->board;
	int color = (node->color == 1) ? 2 : 1;
	int consecutive_pass = 0;
	int max_moves = 361 * 3;
	for (int move = 0; move < max_moves; move++){
		u64 empty[6];
		get_empty(&b, empty);

		int legal[361][2];
		int count = 0;
		for (int i = 0; i < 6; i++) {
			u64 e = empty[i];
			while (e) {
				int bit = __builtin_ctzll(e);
				int pos = i * 64 + bit;
				int r = pos / 19, c = pos % 19;
				if (is_legal_move(&b, r, c, color, 0)){
					legal[count][0] = r; legal[count][1] = c; count++;
				}
				e &= e - 1;
			}
		}

		if (count == 0) {
			consecutive_pass++;
			if (consecutive_pass >= 2) break;
		} else {
			consecutive_pass = 0;
			int best_score = -1;
			int best[361][2], best_count = 0;
			for (int i = 0; i < count; i++) {
				int s = move_score(&b, legal[i][0], legal[i][1], color);
				if (s > best_score) { best_score = s; best_count = 0; }
				if (s == best_score) {
					best[best_count][0] = legal[i][0];
					best[best_count][1] = legal[i][1];
					best_count++;
				}
			}
			int idx = rand() % best_count;
			int r = best[idx][0], c = best[idx][1];
			set_stone(&b, r, c, color);
			check_captures(&b, r, c, (color == 1) ? 2 : 1);
		}
		color = (color == 1) ? 2 : 1;
	}
	int black = 0, white = 0;
	for (int r = 0; r < 19; r++){
		for (int c = 0; c < 19; c++){
			int s = get_stone(&b, r, c);
			black += (s == 1);
			white += (s == 2);
		}
	}
	black += count_territory(&b, 1);
	white += count_territory(&b, 2);
	return black > white ? 1 : 0;
}

void mcts_backprop(MCTSNode *node, int result) {
	while (node != NULL){
		node->visit++;
		node->win += (node->color != result);
		node = node->parent;
	}
}


MCTSNode* mcts_best_move(Board *b, int color, int iterations, int *out_row, int *out_col) {
	MCTSNode *root = mcts_new_node(b, -1, -1, color, NULL);
	mcts_expand(root);

	if (root->num_children == 0) return NULL;

	for (int i = 0; i < iterations; i++) {
		MCTSNode *node = mcts_select(root);
		if (node->visit > 0 && node->num_children == 0) mcts_expand(node);
		node = mcts_select(node);
		int result = mcts_simulate(node);
		mcts_backprop(node, result);
	}

	MCTSNode *best = root->children[0];
	for (int i = 1; i < root->num_children; i++){
		if (root->children[i]->visit > best->visit){
			 best = root->children[i];
		}
	}
	*out_row = best->move_row;
	*out_col = best->move_col;
	return best;
}
