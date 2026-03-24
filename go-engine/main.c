#include <stdio.h>
#include <string.h>
#include "bitboard.h"

// 掃描對手棋子，提掉氣數為 0 的群
void check_captures(Board *b, int row, int col, int opponent) {
    // 找剛落子位置的鄰接格
    int pos = row * 19 + col;
    u64 stone[6] = {0};
    stone[pos >> 6] |= (1ULL << (pos & 63));

    u64 adj[6] = {0};
    get_neighbors(stone, adj);

    // 對每個鄰接的對手棋子做 flood_fill + 數氣
    u64 *opp_board = (opponent == 1) ? b->black : b->white;
    u64 visited[6] = {0};

    for (int i = 0; i < 6; i++) {
        u64 opp_neighbors = adj[i] & opp_board[i] & ~visited[i];
        while (opp_neighbors) {
            int bit = __builtin_ctzll(opp_neighbors);
            int p = i * 64 + bit;

            u64 start[6] = {0};
            start[p >> 6] |= (1ULL << (p & 63));

            u64 group[6] = {0};
            flood_fill(opp_board, start, group);

            if (count_liberties(b, group) == 0)
                capture_group(b, group, opponent);

            // 標記整群為已訪問
            for (int k = 0; k < 6; k++)
                visited[k] |= group[k];

            opp_neighbors &= opp_neighbors - 1;
        }
    }
}

int main(void) {
    init_zobrist();

    Board b = {0};
    int color = 1;        // 1=黑先
    u64 prev_hash = 0;
    int consecutive_pass = 0;

    print_board(&b);

    char input[32];
    while (1) {
        printf("%s to move (row col or 'pass'): ", color == 1 ? "Black" : "White");
        if (!fgets(input, sizeof(input), stdin)) break;

        // pass
        if (strncmp(input, "pass", 4) == 0) {
            consecutive_pass++;
            if (consecutive_pass >= 2) {
                printf("Game over.\n");
                break;
            }
            color = (color == 1) ? 2 : 1;
            continue;
        }
        consecutive_pass = 0;

        int row, col;
        if (sscanf(input, "%d %d", &row, &col) != 2) {
            printf("Invalid input.\n");
            continue;
        }
        if (row < 0 || row > 18 || col < 0 || col > 18) {
            printf("Out of bounds.\n");
            continue;
        }
        if (!is_legal_move(&b, row, col, color, prev_hash)) {
            printf("Illegal move.\n");
            continue;
        }

        prev_hash = b.hash;
        set_stone(&b, row, col, color);

        int opponent = (color == 1) ? 2 : 1;
        check_captures(&b, row, col, opponent);

        print_board(&b);
        color = opponent;
    }

    return 0;
}
