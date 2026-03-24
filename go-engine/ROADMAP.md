# 圍棋引擎開發路線圖

## 目前進度

- [ ] Step 1: Bitboard 基礎（set / clear / get）
- [ ] Step 2: 找鄰接格（neighbor mask）
- [ ] Step 3: Flood Fill（找連通塊 / 整群棋子）
- [ ] Step 4: 數氣（Liberty counting）
- [ ] Step 5: 提子（Capture）
- [ ] Step 6: 落子合法性檢查（自殺手、Ko）
- [ ] Step 7: 遊戲主迴圈（輪流落子、終局）

---

## Step 1：Bitboard 基礎

**檔案**：`bitboard.h` / `bitboard.c`

實作三個函式：
- `set_stone(Board *b, int row, int col, int color)`
- `clear_stone(Board *b, int row, int col, int color)`
- `get_stone(Board *b, int row, int col)`

**核心計算**：
```
pos = row * 19 + col
idx = pos >> 6        // 選哪個 uint64
bit = pos & 63        // 選哪個 bit

set:   board[idx] |=  (1ULL << bit)
clear: board[idx] &= ~(1ULL << bit)
get:   (board[idx] >> bit) & 1
```

編譯測試：
```sh
gcc bitboard.c main.c -o go && ./go
```

---

## Step 2：找鄰接格

每個位置有上下左右四個鄰居，用 bitboard shift 操作找出來：

```
上：pos - 19  →  board >> 19（需遮掉第 0 行的溢出）
下：pos + 19  →  board << 19（需遮掉第 18 行的溢出）
左：pos - 1   →  board >> 1 （需遮掉最右欄的溢出）
右：pos + 1   →  board << 1 （需遮掉最左欄的溢出）
```

難點是邊界 mask，防止棋盤左右兩側「穿越」到下一行。

---

## Step 3：Flood Fill（找連通塊）

給定一個落子位置，找出與它相連的所有同色棋子：

```c
u64 group[6];  // 從單一棋子出發
// 反覆擴展：group |= neighbors(group) & same_color
// 直到 group 不再變化（fixed point）
```

這是圍棋引擎最核心的操作，提子、數氣、眼位判斷都依賴它。

---

## Step 4：數氣

氣 = 連通塊的空白鄰接格數量：

```c
u64 liberties = neighbors(group) & empty_board;
int liberty_count = popcount(liberties);
```

---

## Step 5：提子

當一塊棋子氣數為 0，從棋盤上清除：

```c
if (liberty_count == 0)
    board &= ~group;  // 清掉整塊
```

---

## Step 6：合法性檢查

- **自殺手**：落子後自己的氣為 0（且沒有吃掉對方）→ 非法
- **Ko**：落子後盤面與上一手完全相同 → 非法（用 Zobrist Hash 比較）

---

## Step 7：遊戲主迴圈

```
while (遊戲未結束) {
    讀入落子座標
    合法性檢查
    set_stone
    提子
    切換玩家
    印出盤面
}
```

---

## 建議開發順序

每個 Step 完成後，在 `main.c` 寫 assert 測試，確認正確再往下。
不要一次寫完再測，圍棋的 bug 很難回溯。
