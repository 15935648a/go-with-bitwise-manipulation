#include <stdio.h>
#include "bitboard.h"
#include "mcts.h"

#define MCTS_ITERATIONS 200

int main(void) {
    init_zobrist();

    Board b = {0};
    int color = 1;
    u64 prev_hash = 0;
    int consecutive_pass = 0;

    // 兩個 AI 各自維護一棵樹
    MCTSNode *root = NULL;

    print_board(&b);

    while (1) {
        printf("%s (AI) thinking...\n", color == 1 ? "Black" : "White");

        int row, col;
        MCTSNode *chosen = mcts_best_move(&b, color, MCTS_ITERATIONS, &row, &col);

        if (!chosen) {
            printf("%s passes.\n", color == 1 ? "Black" : "White");
            if (root) { mcts_free(root); root = NULL; }
            consecutive_pass++;
            if (consecutive_pass >= 2) { printf("Game over.\n"); break; }
            color = (color == 1) ? 2 : 1;
            continue;
        }

        consecutive_pass = 0;
        printf("%s plays: %d %d\n", color == 1 ? "Black" : "White", row, col);

        // 釋放舊樹，保留選中的子節點作為下一輪的根
        if (root) mcts_free(root);
        root = chosen;
        root->parent = NULL;

        prev_hash = b.hash;
        set_stone(&b, row, col, color);
        check_captures(&b, row, col, (color == 1) ? 2 : 1);
        print_board(&b);
        color = (color == 1) ? 2 : 1;
    }

    if (root) mcts_free(root);
    return 0;
}
