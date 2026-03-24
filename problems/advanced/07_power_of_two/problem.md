# [ADVANCED] 題目：判斷 2 的冪次

實作函式 `int is_power_of_two(int n)`，判斷 n 是否為 2 的冪次。
是則回傳 1，否則回傳 0。

## 函式簽名

```c
int is_power_of_two(int n);
```

## 測試案例

```
is_power_of_two(1)   → 1    // 2^0
is_power_of_two(2)   → 1    // 2^1
is_power_of_two(4)   → 1    // 2^2
is_power_of_two(16)  → 1    // 2^4
is_power_of_two(0)   → 0    // 0 不是 2 的冪次
is_power_of_two(3)   → 0
is_power_of_two(6)   → 0
is_power_of_two(-4)  → 0    // 負數不是 2 的冪次
```

## 限制

- 禁止使用 `if/else`、`%`、迴圈
- 必須使用位元運算完成
- 解法必須為 O(1)

## 提示

思考：2 的冪次在二進位表示下有什麼特殊結構？
比較 n 和 n-1 的位元樣式，能發現什麼規律？

```
n     = 8  → 0b 0000 1000
n-1   = 7  → 0b 0000 0111
n & (n-1)  → 0b 0000 0000  ← 結果為 0！

n     = 6  → 0b 0000 0110
n-1   = 5  → 0b 0000 0101
n & (n-1)  → 0b 0000 0100  ← 結果不為 0
```

## 實際應用場景

| 場景 | 用途 |
|------|------|
| **記憶體對齊** | OS/allocator 檢查 buffer size 是否為 2 的冪（方便位元 mask 對齊） |
| **Hash Table** | bucket 數量必須是 2 的冪，才能用 `& (n-1)` 取代 `% n` |
| **GPU Texture** | CUDA/OpenGL texture 尺寸需為 2 的冪次才能啟用 mipmapping |
| **FFT 演算法** | Cooley-Tukey FFT 要求輸入長度為 2 的冪次 |
| **Bitboard** | 圍棋/西洋棋引擎中驗證棋盤區塊大小 |
