# [BASIC] 題目：設置 / 清除 / 翻轉第 k 位元

實作以下三個函式，對整數 `n` 的第 `k` 個位元進行修改（位元編號從 **0** 開始，0 為 LSB）：

```c
int set_bit   (int n, int k);  // 將第 k 位設為 1
int clear_bit (int n, int k);  // 將第 k 位設為 0
int toggle_bit(int n, int k);  // 將第 k 位翻轉（0→1 或 1→0）
```

---

## 測試案例

```
set_bit   (0b1010, 0) → 0b1011   // 第 0 位從 0 變 1
set_bit   (0b1010, 1) → 0b1010   // 第 1 位已是 1，不變
set_bit   (0b0000, 3) → 0b1000

clear_bit (0b1010, 1) → 0b1000   // 第 1 位從 1 變 0
clear_bit (0b1010, 0) → 0b1010   // 第 0 位已是 0，不變
clear_bit (0b1111, 3) → 0b0111

toggle_bit(0b1010, 0) → 0b1011   // 0 → 1
toggle_bit(0b1010, 1) → 0b1000   // 1 → 0
toggle_bit(0b1010, 3) → 0b0010   // 1 → 0
```

---

## 輸入格式

每行格式：`<op> <n> <k>`，其中 `op` 為 `S`（set）、`C`（clear）、`T`（toggle）。

```
S 10 0
S 10 1
S 0 3
C 10 1
C 10 0
C 15 3
T 10 0
T 10 1
T 10 3
```

## 輸出格式

每行輸出十進位結果。

```
11
10
8
8
10
7
11
8
2
```

---

## 限制

- 禁止使用 `if/else`
- 禁止使用 `%`、`/`
- `0 <= k <= 30`

---

## 提示

三個操作各對應一種 operator：

| 操作 | 關鍵 operator | 搭配的 mask |
|------|--------------|-------------|
| set  | `\|`         | `1 << k`    |
| clear| `&`          | `~(1 << k)` |
| toggle | `^`        | `1 << k`    |

---

## 真實應用場景（Real-World Usage）

這三個操作是位元操作的「CRUD」，在真實系統中無處不在：

### 1. Linux 檔案系統權限（`chmod`）
```c
// stat.st_mode 是一個位元欄位
// 設定 owner 可執行位 (bit 6)
mode = set_bit(mode, 6);   // 等同 mode |= S_IXUSR

// 移除 group write 位 (bit 4)
mode = clear_bit(mode, 4); // 等同 mode &= ~S_IWGRP
```
Linux kernel 的 `inode` 就用 16-bit 整數存讀/寫/執行權限。

### 2. GPU Warp 狀態遮罩（CUDA）
```c
// CUDA warp 有 32 個 thread，用 uint32_t 的每個 bit 代表一個 thread 是否 active
uint32_t active_mask = 0xFFFFFFFF;

// 讓 thread 5 休眠（clear）
active_mask = clear_bit(active_mask, 5);

// 喚醒 thread 5（set）
active_mask = set_bit(active_mask, 5);

// __ballot_sync / __activemask() 回傳的就是這種 mask
```

### 3. 棋盤遊戲的 Bitboard（西洋棋引擎）
```c
// 64-bit uint64_t 表示整個棋盤，每個 bit 代表一個格子
uint64_t white_pawns = 0;

// 在 e2（bit 12）放一個白兵
white_pawns = set_bit(white_pawns, 12);

// 白兵移動後清除 e2，設置 e4
white_pawns = clear_bit(white_pawns, 12);
white_pawns = set_bit(white_pawns, 28);
```
Stockfish 等頂尖西洋棋引擎全程使用 bitboard，move generation 每秒可算數千萬個局面。

### 4. 網路封包標誌位（TCP Flags）
```c
// TCP header flags: URG ACK PSH RST SYN FIN（低 6 位）
uint8_t flags = 0;
flags = set_bit(flags, 1);  // SYN = 1  → 建立連線
flags = set_bit(flags, 4);  // ACK = 1  → 確認
// flags = 0b010010 = SYN-ACK

// 收到 RST 就 toggle debug log 開關（常見 debug 技巧）
debug_flag = toggle_bit(debug_flag, 0);
```

### 5. 動態規劃的狀態壓縮（Bitmask DP）
```c
// TSP（旅行商問題）：用 bitmask 記錄已拜訪的城市集合
int visited = 0;
visited = set_bit(visited, city);    // 標記城市已拜訪
visited = clear_bit(visited, city);  // 回溯時取消
bool was_visited = get_bit(visited, city);  // 查詢
```
LeetCode 上大量 DP 題（如 #847, #943, #1349）的關鍵就是這三個操作。
