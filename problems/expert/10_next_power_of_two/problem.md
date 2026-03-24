# [EXPERT] 題目：找出大於等於 n 的最小 2 的冪次

給定正整數 n，回傳**大於等於 n 的最小 2 的冪次**。

## 函式簽名

```c
unsigned int next_power_of_two(unsigned int n);
```

## 測試案例

```
next_power_of_two(1)  → 1     // 2^0，本身就是 2 的冪
next_power_of_two(2)  → 2     // 2^1，本身就是 2 的冪
next_power_of_two(3)  → 4     // 下一個是 4
next_power_of_two(5)  → 8
next_power_of_two(8)  → 8     // 本身就是 2 的冪
next_power_of_two(9)  → 16
next_power_of_two(100)→ 128
```

## 限制

- 禁止使用 `if/else`、迴圈、`log`、`pow`
- 只能使用位元運算與加減法
- 必須為 O(1)

## 提示

思考：如果把最高位的 1 右邊所有位元都變成 1，再加 1，會發生什麼事？

```
n = 5 → 0b 0000 0101
把最高位右邊全填滿 1：
        0b 0000 0111
加 1：  0b 0000 1000 = 8 ✓
```

如何「把最高位右邊全填滿 1」？試試用 `n |= n >> k` 的系列操作...

但要注意：n 本身如果已經是 2 的冪，這樣做會多加一級。先 `n--` 再操作可以解決這個問題。

## 實際應用場景

| 場景 | 用途 |
|------|------|
| **Hash Table** | bucket 數必須是 2 的冪，才能用 `& (n-1)` 取代 `% n` |
| **GPU Texture** | CUDA/OpenGL texture 尺寸必須是 2 的冪才能 mipmap |
| **記憶體 Allocator** | buddy system allocator 以 2 的冪為單位分配 |
| **圍棋引擎** | Transposition table 大小設為 2 的冪，加速 hash lookup |
| **FFT** | 輸入長度補齊到最近的 2 的冪次（zero-padding） |
