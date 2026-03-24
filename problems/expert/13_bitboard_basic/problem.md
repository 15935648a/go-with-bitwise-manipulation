# [EXPERT] 題目：19x19 Bitboard 基礎操作

19x19 圍棋盤有 361 個交叉點，無法放入單一 `uint64_t`（只有 64 bits）。
標準做法是用 **6 個 `uint64_t`** 陣列表示（6 × 64 = 384 bits，足夠放 361 個位置，尾端 23 bits 不使用）。

```
board[0] : bit 0  ~ bit 63   → 盤面位置 0  ~ 63
board[1] : bit 0  ~ bit 63   → 盤面位置 64 ~ 127
board[2] : bit 0  ~ bit 63   → 盤面位置 128 ~ 191
board[3] : bit 0  ~ bit 63   → 盤面位置 192 ~ 255
board[4] : bit 0  ~ bit 63   → 盤面位置 256 ~ 319
board[5] : bit 0  ~ bit 40   → 盤面位置 320 ~ 360（bit 41~63 不使用）
```

盤面位置編號：`pos = row * 19 + col`（row, col 從 0 起算）

## 實作以下函式

```c
typedef unsigned long long u64;

// 在 (row, col) 放一顆棋子（設置對應 bit 為 1）
void set_stone(u64 board[6], int row, int col);

// 移除 (row, col) 的棋子（清除對應 bit）
void clear_stone(u64 board[6], int row, int col);

// 查詢 (row, col) 是否有棋子（回傳 0 或 1）
int get_stone(u64 board[6], int row, int col);
```

## 測試案例

```
空盤後 set_stone(board, 0, 0)  → get_stone(board, 0, 0) = 1
空盤後 set_stone(board, 18, 18)→ get_stone(board, 18, 18) = 1
set 後 clear_stone(board, 0, 0)→ get_stone(board, 0, 0) = 0
get_stone(空盤, 3, 3)          → 0
```

## 限制

- 只能用 `&`、`|`、`~`、`<<`、`>>` 操作
- 禁止使用迴圈掃描每個 bit

## 提示

**pos → 陣列索引**：
```
pos = row * 19 + col
陣列索引 idx = pos / 64   → 用位元運算：pos >> 6
bit 位置 bit = pos % 64   → 用位元運算：pos & 63
```

**操作對應**：
```
set:   board[idx] |=  (1ULL << bit)
clear: board[idx] &= ~(1ULL << bit)
get:   (board[idx] >> bit) & 1
```

注意：必須用 `1ULL`（unsigned long long），不能用 `1`（int），否則左移超過 31 位會 undefined behavior。

## 實際應用場景

這就是圍棋引擎的最底層資料結構。所有後續操作（找鄰接格、flood fill、數氣）都建立在這三個函式上。
