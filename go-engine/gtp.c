#include "gtp.h"
#include "bitboard.h"
#include "mcts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// GTP 欄位字母（跳過 I）
static const char COL_LETTERS[] = "ABCDEFGHJKLMNOPQRST";

static int gtp_to_col(char c) {
	c = toupper((unsigned char)c);
	for (int i = 0; i < 19; i++)
		if (COL_LETTERS[i] == c) return i;
	return -1;
}

// GTP row 1 = 棋盤最下排 = 我們的 row 18
static int gtp_to_row(int n) { return 19 - n; }

static void make_vertex(int row, int col, char *out) {
	sprintf(out, "%c%d", COL_LETTERS[col], 19 - row);
}

static int parse_color(const char *s) {
	if (strcasecmp(s, "black") == 0 || strcasecmp(s, "b") == 0) return 1;
	if (strcasecmp(s, "white") == 0 || strcasecmp(s, "w") == 0) return 2;
	return 0;
}

void gtp_loop(double time_limit_ms) {
	Board b    = {0};
	MCTSNode *root = NULL;
	char line[512];

	while (fgets(line, sizeof(line), stdin)) {
		// 去掉行尾空白
		int len = (int)strlen(line);
		while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r' || line[len-1] == ' '))
			line[--len] = '\0';
		if (len == 0) continue;
		if (line[0] == '#') continue;  // 跳過註解

		// 解析可選的 ID 數字
		int id = -1;
		char *p = line;
		if (isdigit((unsigned char)*p)) {
			id = atoi(p);
			while (*p && isdigit((unsigned char)*p)) p++;
			while (*p == ' ') p++;
		}

		char cmd[64] = {0}, a1[64] = {0}, a2[64] = {0};
		sscanf(p, "%63s %63s %63s", cmd, a1, a2);

// GTP 回應 helper
#define OK(msg)  do { \
	if (id >= 0) printf("=%d %s\n\n", id, (msg)); \
	else         printf("= %s\n\n",        (msg)); \
	fflush(stdout); } while(0)

#define ERR(msg) do { \
	if (id >= 0) printf("?%d %s\n\n", id, (msg)); \
	else         printf("? %s\n\n",        (msg)); \
	fflush(stdout); } while(0)

		if (strcmp(cmd, "protocol_version") == 0) {
			OK("2");

		} else if (strcmp(cmd, "name") == 0) {
			OK("BitGo");

		} else if (strcmp(cmd, "version") == 0) {
			OK("1.0");

		} else if (strcmp(cmd, "known_command") == 0) {
			OK("true");

		} else if (strcmp(cmd, "list_commands") == 0) {
			OK("protocol_version\nname\nversion\nknown_command\nlist_commands\nboardsize\nclear_board\nkomi\nplay\ngenmove\nquit");

		} else if (strcmp(cmd, "boardsize") == 0) {
			if (atoi(a1) != 19) ERR("only 19x19 supported");
			else                OK("");

		} else if (strcmp(cmd, "clear_board") == 0) {
			memset(&b, 0, sizeof(b));
			if (root) { mcts_free(root); root = NULL; }
			OK("");

		} else if (strcmp(cmd, "komi") == 0) {
			// 貼目：目前不影響內部計算，直接接受
			OK("");

		} else if (strcmp(cmd, "play") == 0) {
			int color = parse_color(a1);
			if (color == 0) { ERR("invalid color"); continue; }

			if (strcasecmp(a2, "pass") == 0) {
				if (root) { mcts_free(root); root = NULL; }
				OK("");
				continue;
			}

			int col = gtp_to_col(a2[0]);
			int row = gtp_to_row(atoi(a2 + 1));
			if (col < 0 || row < 0 || row >= 19) { ERR("invalid vertex"); continue; }

			// 嘗試在樹上前進
			if (root) {
				MCTSNode *next = mcts_advance(root, row, col);
				root = next;
			}
			set_stone(&b, row, col, color);
			check_captures(&b, row, col, (color == 1) ? 2 : 1);
			OK("");

		} else if (strcmp(cmd, "genmove") == 0) {
			int color = parse_color(a1);
			if (color == 0) { ERR("invalid color"); continue; }

			int row, col;
			MCTSNode *chosen = mcts_best_move(&b, color, time_limit_ms, &row, &col);

			if (!chosen) {
				if (root) { mcts_free(root); root = NULL; }
				OK("pass");
				continue;
			}

			set_stone(&b, row, col, color);
			check_captures(&b, row, col, (color == 1) ? 2 : 1);

			if (root) mcts_free(root);
			root = chosen;
			root->parent = NULL;

			char vertex[8];
			make_vertex(row, col, vertex);
			OK(vertex);

		} else if (strcmp(cmd, "quit") == 0) {
			OK("");
			if (root) mcts_free(root);
			break;

		} else {
			ERR("unknown command");
		}

#undef OK
#undef ERR
	}
}
