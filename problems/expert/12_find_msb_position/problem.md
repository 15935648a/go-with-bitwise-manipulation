# [EXPERT] 題目：找最高位的 1 的位置

實作函式 `int find_msb_pos(unsigned int n)`，回傳 n 的最高位 1 所在的位元索引（從 0 起算）。
若 n = 0，回傳 -1。

## 函式簽名

```c
int find_msb_pos(unsigned int n);
```

## 測試案例

```
find_msb_pos(1)   → 0    // 0b0001，最高位 1 在第 0 位
find_msb_pos(4)   → 2    // 0b0100
find_msb_pos(20)  → 4    // 0b10100，最高位 1 在第 4 位
find_msb_pos(255) → 7    // 0b11111111
find_msb_pos(0)   → -1
```

## 限制

- 實作兩個版本：
  1. **手刻版**：只用位元運算（不能用 `__builtin_clz`）
  2. **builtin 版**：使用 `__builtin_clz`
- n = 0 兩個版本都需正確處理

## `__builtin_clz` 介紹

**CLZ = Count Leading Zeros**，數從最高位開始連續有幾個 0：

```
n = 20 = 0b 0001 0100
leading zeros = 27
__builtin_clz(20) → 27
```

MSB 位置 = 31 - __builtin_clz(n)（對 32 位元整數）

注意：`__builtin_clz(0)` 是 undefined behavior。

## 提示

**手刻版**：你在 10 題做過 bit smearing（把最高位右邊全填 1），填完之後 `popcount - 1` 就是位置。

```
n = 20 → 0b 0001 0100
填滿    → 0b 0001 1111
popcount = 5
位置 = 5 - 1 = 4  ✓
```

## 實際應用場景

| 場景 | 用途 |
|------|------|
| **整數 log2** | `floor(log2(n)) = 31 - __builtin_clz(n)`，O(1) 無浮點 |
| **圍棋引擎** | 找 bitboard 中最高位棋子的位置，掃描方向相反時用 MSB |
| **Huffman 編碼** | 決定編碼長度需要 floor(log2) |
| **網路 IP mask** | 計算 subnet 前綴長度 |
