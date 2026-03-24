#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitboard.h"
#include "mcts.h"
#include "gtp.h"

#define MCTS_TIME_MS  200.0
#define BENCH_MCTS_TIME_MS 1.0
#define PATTERN_FILE  "pattern_table.bin"

int main(int argc, char *argv[]) {
    init_zobrist();
    srand(42);

    // --gtp：GTP 協議模式（對弈用）
    if (argc >= 2 && strcmp(argv[1], "--gtp") == 0) {
        if (load_pattern_table(PATTERN_FILE))
            fprintf(stderr, "Pattern table loaded.\n");
        gtp_loop(MCTS_TIME_MS);
        return 0;
    }

    // --bench N：跑多組對戰，測試 heuristic / MCTS 的相對棋力
    if (argc >= 2 && strcmp(argv[1], "--bench") == 0) {
        int games = (argc >= 3) ? atoi(argv[2]) : 200;
        if (!load_pattern_table(PATTERN_FILE)) {
            printf("No pattern table found. Run --train first.\n");
            return 1;
        }
        const struct {
            const char *name;
            int black_policy;
            int white_policy;
            int tracked_color;
            double mcts_time_ms;
        } suites[] = {
            {"greedy+pattern vs random", 2, 0, 1, 0.0},
            {"greedy+pattern vs greedy(no pattern)", 2, 1, 1, 0.0},
            {"mcts+pattern vs random", 4, 0, 1, BENCH_MCTS_TIME_MS},
            {"mcts+pattern vs greedy(no pattern)", 4, 1, 1, BENCH_MCTS_TIME_MS},
            {"mcts+pattern vs mcts(no pattern)", 4, 3, 1, BENCH_MCTS_TIME_MS},
        };
        int suite_count = (int)(sizeof(suites) / sizeof(suites[0]));
        int wins[5] = {0};

        for (int i = 0; i < games; i++) {
            for (int s = 0; s < suite_count; s++) {
                int black_policy = suites[s].black_policy;
                int white_policy = suites[s].white_policy;
                int tracked_color = suites[s].tracked_color;

                if (i % 2 == 1) {
                    int tmp = black_policy;
                    black_policy = white_policy;
                    white_policy = tmp;
                    tracked_color = (tracked_color == 1) ? 2 : 1;
                }

                wins[s] += (bench_one_game(black_policy, white_policy, suites[s].mcts_time_ms) == tracked_color);
            }
        }

        printf("Bench suites (%d games each, MCTS %.0f ms/move)\n", games, BENCH_MCTS_TIME_MS);
        for (int s = 0; s < suite_count; s++) {
            double win_rate = (games > 0) ? (100.0 * wins[s] / games) : 0.0;
            printf("  %s: %d / %d = %.1f%%\n",
                   suites[s].name, wins[s], games, win_rate);
        }
        return 0;
    }

    // --train N：跑 N 局自弈，存 pattern table 後結束
    if (argc >= 2 && strcmp(argv[1], "--train") == 0) {
        int games = (argc >= 3) ? atoi(argv[2]) : 10000;
        load_pattern_table(PATTERN_FILE);  // 繼續累積舊資料
        printf("Training %d games...\n", games);
        for (int i = 0; i < games; i++) {
            train_one_game();
            if ((i + 1) % 1000 == 0)
                printf("  %d / %d\n", i + 1, games);
        }
        save_pattern_table(PATTERN_FILE);
        printf("Saved to %s\n", PATTERN_FILE);
        return 0;
    }

    // 一般對弈模式：嘗試載入 pattern table
    if (load_pattern_table(PATTERN_FILE))
        printf("Pattern table loaded.\n");
    else
        printf("No pattern table found (run --train first).\n");

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
        MCTSNode *chosen = mcts_best_move(&b, color, MCTS_TIME_MS, &row, &col);

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
