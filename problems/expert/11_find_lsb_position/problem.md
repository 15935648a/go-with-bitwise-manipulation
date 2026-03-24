# [EXPERT] 題目：找最低位的 1 的位置

實作函式 `int find_lsb_pos(unsigned int n)`，回傳 n 的最低位 1 所在的位元索引（從 0 起算）。
若 n = 0，回傳 -1。

## 函式簽名

```c
int find_lsb_pos(unsigned int n);
```

## 測試案例

```
find_lsb_pos(1)   → 0    // 0b0001，最低位 1 在第 0 位
find_lsb_pos(4)   → 2    // 0b0100，最低位 1 在第 2 位
find_lsb_pos(20)  → 2    // 0b10100，最低位 1 在第 2 位
find_lsb_pos(16)  → 4    // 0b10000
find_lsb_pos(0)   → -1   // 特殊處理
```

## 限制

- 必須實作兩個版本：
  1. **手刻版**：只用位元運算（不能用 `__builtin_ctz`）
  2. **builtin 版**：使用 `__builtin_ctz`
- 禁止使用迴圈、`if/else`（手刻版）
- n = 0 的處理兩個版本都需要正確

## 提示

**手刻版**：`n & -n` 可以取出最低位的 1，再想辦法把它的位置算出來。

位置的計算可以結合你學過的某個函式...

**builtin 版**：注意 `__builtin_ctz(0)` 是 undefined behavior，n=0 要先處理掉。

## 實際應用場景

| 場景 | 用途 |
|------|------|
| **圍棋 Bitboard 遍歷** | `ctz` 找到棋子位置，`n & (n-1)` 清掉，迴圈掃全盤 |
| **Linux kernel** | `__ffs()`（find first set）大量用於 bitmask 遍歷 |
| **編譯器優化** | 除以 2 的冪次：`x >> __builtin_ctz(divisor)` |
| **記憶體對齊檢查** | `ctz(ptr)` 得知指標對齊到幾 byte 邊界 |
