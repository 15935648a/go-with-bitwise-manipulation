#include "mcts.h"
#include "bitboard.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>

// Pattern table: 8 鄰格 × 2 bits = 16 bits → 65536 entries
int pat_win[1 << 16]   = {0};
int pat_visit[1 << 16] = {0};

enum {
	POLICY_RANDOM = 0,
	POLICY_GREEDY_NO_PATTERN = 1,
	POLICY_GREEDY_WITH_PATTERN = 2,
	POLICY_MCTS_NO_PATTERN = 3,
	POLICY_MCTS_WITH_PATTERN = 4
};

// 把 (r,c) 周圍 8 格的狀態壓成 16-bit key（從落子者視角）
// 0=空, 1=自己, 2=對手；中心格跳過
static unsigned int encode_3x3(Board *b, int r, int c, int color) {
	unsigned int key = 0;
	int idx = 0;
	for (int dr = -1; dr <= 1; dr++) {
		for (int dc = -1; dc <= 1; dc++) {
			if (dr == 0 && dc == 0) continue;
			int nr = r + dr, nc = c + dc;
			int s = 0;
			if (nr >= 0 && nr < 19 && nc >= 0 && nc < 19)
				s = get_stone(b, nr, nc);
			if      (s == color) s = 1;
			else if (s != 0)     s = 2;
			key |= ((unsigned int)s << (idx * 2));
			idx++;
		}
	}
	return key;
}

void save_pattern_table(const char *filename) {
	FILE *f = fopen(filename, "wb");
	if (!f) return;
	fwrite(pat_win,   sizeof(pat_win),   1, f);
	fwrite(pat_visit, sizeof(pat_visit), 1, f);
	fclose(f);
}

int load_pattern_table(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if (!f) return 0;
	fread(pat_win,   sizeof(pat_win),   1, f);
	fread(pat_visit, sizeof(pat_visit), 1, f);
	fclose(f);
	return 1;
}


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


// 判斷 (r,c) 是否為 color 的眼（四個正交鄰格全是自己的子或邊界）
static int is_eye(Board *b, int r, int c, int color) {
	const int dr[] = {-1, 1,  0, 0};
	const int dc[] = { 0, 0, -1, 1};
	for (int d = 0; d < 4; d++) {
		int nr = r + dr[d], nc = c + dc[d];
		if (nr < 0 || nr >= 19 || nc < 0 || nc >= 19) continue;
		if (get_stone(b, nr, nc) != color) return 0;
	}
	return 1;
}

static int move_score(Board *b, int r, int c, int color, int use_pattern) {
	int score = 0;
	int opponent = (color == 1) ? 2 : 1;

	int pos = r * 19 + c;
	u64 stone[6] = {0};
	stone[pos >> 6] |= (1ULL << (pos & 63));
	u64 adj[6] = {0};
	get_neighbors(stone, adj);

	// Pattern table bonus（需先跑 --train）
	if (use_pattern && pat_visit[encode_3x3(b, r, c, color)] >= 30) {
		unsigned int key = encode_3x3(b, r, c, color);
		score += (int)((double)pat_win[key] / pat_visit[key] * 10) - 5;
	}

	// 逃脫打吃：落子前，檢查相鄰我方棋群是否只剩 1 氣（被打吃）
	u64 *my_board_pre = (color == 1) ? b->black : b->white;
	int rescued = 0;
	for (int i = 0; i < 6 && !rescued; i++) {
		u64 my_adj = adj[i] & my_board_pre[i];
		while (my_adj) {
			int bit = __builtin_ctzll(my_adj);
			int p   = i * 64 + bit;
			u64 s[6] = {0};
			s[p >> 6] |= (1ULL << (p & 63));
			u64 g[6] = {0};
			flood_fill(my_board_pre, s, g);
			if (count_liberties(b, g) == 1) { score += 8; rescued = 1; break; }
			my_adj &= my_adj - 1;
		}
	}

	// 落子
	Board tmp = *b;
	set_stone(&tmp, r, c, color);

	// 提子 & 打吃：掃描相鄰對手棋群
	u64 *opp_board = (opponent == 1) ? tmp.black : tmp.white;
	for (int i = 0; i < 6; i++){
		u64 opp = adj[i] & opp_board[i];
		while (opp) {
			int bit = __builtin_ctzll(opp);
			int p   = i * 64 + bit;
			u64 s[6] = {0};
			s[p >> 6] |= (1ULL << (p & 63));
			u64 g[6] = {0};
			flood_fill(opp_board, s, g);
			int libs = count_liberties(&tmp, g);
			if      (libs == 0) score += 10;  // 提子
			else if (libs == 1) score += 5;   // 打吃
			opp &= opp - 1;
		}
	}

	// 我方棋群的氣數（落子後）
	u64 *my_board = (color == 1) ? tmp.black : tmp.white;
	u64 start[6] = {0};
	start[pos >> 6] |= (1ULL << (pos & 63));
	u64 group[6] = {0};
	flood_fill(my_board, start, group);
	int libs = count_liberties(&tmp, group);
	if      (libs >= 3) score += 3;
	else if (libs == 2) score += 1;

	return score;
}

static int collect_legal_moves(Board *b, int color, int legal[361][2]) {
	u64 empty[6];
	int count = 0;

	get_empty(b, empty);

	for (int i = 0; i < 6; i++) {
		u64 e = empty[i];
		while (e) {
			int bit = __builtin_ctzll(e);
			int pos = i * 64 + bit;
			int r = pos / 19, c = pos % 19;
			if (is_legal_move(b, r, c, color, 0) && !is_eye(b, r, c, color)) {
				legal[count][0] = r;
				legal[count][1] = c;
				count++;
			}
			e &= e - 1;
		}
	}

	return count;
}

static void pick_move_with_policy(Board *b, int color, int policy, int legal[361][2], int count, int *out_r, int *out_c) {
	if (policy == POLICY_RANDOM) {
		int idx = rand() % count;
		*out_r = legal[idx][0];
		*out_c = legal[idx][1];
		return;
	}

	int use_pattern = (policy == POLICY_GREEDY_WITH_PATTERN);
	int best_score = INT_MIN;
	int best[361][2], best_count = 0;

	for (int i = 0; i < count; i++) {
		int s = move_score(b, legal[i][0], legal[i][1], color, use_pattern);
		if (s > best_score) {
			best_score = s;
			best_count = 0;
		}
		if (s == best_score) {
			best[best_count][0] = legal[i][0];
			best[best_count][1] = legal[i][1];
			best_count++;
		}
	}

	int idx = rand() % best_count;
	*out_r = best[idx][0];
	*out_c = best[idx][1];
}

MCTSNode *mcts_new_node(Board *b, int row, int col, int color, MCTSNode *parent){
	MCTSNode *node = calloc(1, sizeof(MCTSNode));
	node->board      = *b;
	node->move_row   = row;
	node->move_col   = col;
	node->color      = color;
	node->parent     = parent;
	return node;
}

static double ucb(MCTSNode *node){
	if(node->visit == 0) return 1e9;

	double mcts_score = (double)node->win / node->visit
		+ 1.41 * sqrt(log(node->parent->visit) / node->visit);

	int pos = node->move_row * 19 + node->move_col;
	if (pos < 0 || pos >= 361 || node->rave_visit[pos] == 0)
		return mcts_score;

	double rave_score = (double)node->rave_win[pos] / node->rave_visit[pos];
	// beta: 當 visit 夠多時趨近 0，RAVE 影響力遞減
	double beta = sqrt(500.0 / (3.0 * node->visit + 500.0));
	return (1.0 - beta) * mcts_score + beta * rave_score;
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

static int mcts_simulate_policy(MCTSNode *node, int *moves_played, int *num_moves, int policy){
	Board b = node->board;
	int color = (node->color == 1) ? 2 : 1;
	int consecutive_pass = 0;
	int max_moves = 361 * 3;
	*num_moves = 0;
	for (int move = 0; move < max_moves; move++){
		int legal[361][2];
		int count = collect_legal_moves(&b, color, legal);

		if (count == 0) {
			consecutive_pass++;
			if (consecutive_pass >= 2) break;
		} else {
			consecutive_pass = 0;
			int r, c;
			pick_move_with_policy(&b, color, policy, legal, count, &r, &c);
			set_stone(&b, r, c, color);
			check_captures(&b, r, c, (color == 1) ? 2 : 1);
			if (*num_moves < 361 * 3)
				moves_played[(*num_moves)++] = r * 19 + c;
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

int mcts_simulate(MCTSNode *node, int *moves_played, int *num_moves){
	return mcts_simulate_policy(node, moves_played, num_moves, POLICY_GREEDY_WITH_PATTERN);
}

void mcts_backprop(MCTSNode *node, int result, int *moves_played, int num_moves) {
	while (node != NULL){
		node->visit++;
		node->win += (node->color != result);
		// RAVE: 把模擬中出現的所有 move 更新到這個節點的 rave 陣列
		for (int i = 0; i < num_moves; i++) {
			int pos = moves_played[i];
			node->rave_visit[pos]++;
			node->rave_win[pos] += (node->color != result);
		}
		node = node->parent;
	}
}


static MCTSNode* mcts_best_move_policy(Board *b, int color, double time_limit_ms, int policy, int *out_row, int *out_col) {
	MCTSNode *root = mcts_new_node(b, -1, -1, color, NULL);
	mcts_expand(root);

	if (root->num_children == 0) return NULL;

	int moves_played[361 * 3];
	int num_moves;

	struct timespec t0, t1;
	clock_gettime(CLOCK_MONOTONIC, &t0);

	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &t1);
		double elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0
		               + (t1.tv_nsec - t0.tv_nsec) / 1e6;
		if (elapsed >= time_limit_ms) break;

		MCTSNode *node = mcts_select(root);
		if (node->visit > 0 && node->num_children == 0) mcts_expand(node);
		node = mcts_select(node);
		int result = mcts_simulate_policy(node, moves_played, &num_moves, policy);
		mcts_backprop(node, result, moves_played, num_moves);
	}

	MCTSNode *best = root->children[0];
	for (int i = 1; i < root->num_children; i++){
		if (root->children[i]->visit > best->visit){
			 best = root->children[i];
		}
	}
	*out_row = best->move_row;
	*out_col = best->move_col;
	return mcts_advance(root, best->move_row, best->move_col);
}

MCTSNode* mcts_best_move(Board *b, int color, double time_limit_ms, int *out_row, int *out_col) {
	return mcts_best_move_policy(b, color, time_limit_ms, POLICY_GREEDY_WITH_PATTERN, out_row, out_col);
}

static int play_one_move(Board *b, int color, int policy, double mcts_time_ms) {
	int legal[361][2];
	int count = collect_legal_moves(b, color, legal);

	if (count == 0) return 0;

	int r, c;
	if (policy == POLICY_MCTS_NO_PATTERN || policy == POLICY_MCTS_WITH_PATTERN) {
		int rollout_policy = (policy == POLICY_MCTS_WITH_PATTERN) ? POLICY_GREEDY_WITH_PATTERN : POLICY_GREEDY_NO_PATTERN;
		MCTSNode *chosen = mcts_best_move_policy(b, color, mcts_time_ms, rollout_policy, &r, &c);
		if (!chosen) return 0;
		mcts_free(chosen);
	} else {
		pick_move_with_policy(b, color, policy, legal, count, &r, &c);
	}

	set_stone(b, r, c, color);
	check_captures(b, r, c, (color == 1) ? 2 : 1);
	return 1;
}

int bench_one_game(int black_policy, int white_policy, double mcts_time_ms) {
	Board b = {0};
	int color = 1;
	int consecutive_pass = 0;

	for (int step = 0; step < 361 * 3; step++) {
		int policy = (color == 1) ? black_policy : white_policy;
		if (!play_one_move(&b, color, policy, mcts_time_ms)) {
			if (++consecutive_pass >= 2) break;
		} else {
			consecutive_pass = 0;
		}
		color = (color == 1) ? 2 : 1;
	}

	int black = 0, white = 0;
	for (int r = 0; r < 19; r++)
		for (int c = 0; c < 19; c++) {
			int s = get_stone(&b, r, c);
			black += (s == 1); white += (s == 2);
		}
	black += count_territory(&b, 1);
	white += count_territory(&b, 2);
	return (black > white) ? 1 : 2;
}

void train_one_game(void) {
	Board b = {0};
	int color = 1;
	int consecutive_pass = 0;

	unsigned int patterns[361 * 3];
	int move_colors[361 * 3];
	int num_moves = 0;

	for (int step = 0; step < 361 * 3; step++) {
		int legal[361][2];
		int count = collect_legal_moves(&b, color, legal);

		if (count == 0) {
			if (++consecutive_pass >= 2) break;
		} else {
			consecutive_pass = 0;
			int r, c;
			// epsilon-greedy: 30% 機率隨機選子，增加訓練多樣性
			if (rand() % 100 < 30) {
				pick_move_with_policy(&b, color, POLICY_RANDOM, legal, count, &r, &c);
			} else {
				pick_move_with_policy(&b, color, POLICY_GREEDY_WITH_PATTERN, legal, count, &r, &c);
			}
			patterns[num_moves]    = encode_3x3(&b, r, c, color);
			move_colors[num_moves] = color;
			num_moves++;
			set_stone(&b, r, c, color);
			check_captures(&b, r, c, (color == 1) ? 2 : 1);
		}
		color = (color == 1) ? 2 : 1;
	}

	// 計分
	int black = 0, white = 0;
	for (int r = 0; r < 19; r++)
		for (int c = 0; c < 19; c++) {
			int s = get_stone(&b, r, c);
			black += (s == 1); white += (s == 2);
		}
	black += count_territory(&b, 1);
	white += count_territory(&b, 2);
	int winner = (black > white) ? 1 : 2;

	// 更新 pattern table
	for (int i = 0; i < num_moves; i++) {
		unsigned int key = patterns[i];
		pat_visit[key]++;
		if (move_colors[i] == winner)
			pat_win[key]++;
	}
}
