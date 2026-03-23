# 05 Branchless Min/Max — 學習筆記

## 解法演進

### v1：純位元模擬比較（sol.c）

不使用任何比較運算子，用 sign bit 手動推導大小關係。

核心思路：
- 異號時：直接從 sign bit 判斷誰大誰小
- 同號時：`diff = a - b`，取 diff 的 sign bit 作為 mask

```c
int diff = a - b;
int sign = diff >> 31;
int same_sign_result = b + (diff & (sign ^ mode));
```

適合理解 CPU 比較指令的底層實作原理，是刻意不用 `<` / `>` 的訓練。

---

### v2：更精簡的業界慣用寫法

```c
int bit_min(int a, int b) { return b ^ ((a ^ b) & -(a < b)); }
int bit_max(int a, int b) { return b ^ ((a ^ b) & -(a > b)); }
```

原理：
- `a < b` 產生 0 或 1
- `-(a < b)` 產生 `0x00000000` 或 `0xFFFFFFFF`
- mask 全 1 時選 a，全 0 時選 b

位元視覺化（min，a=3, b=7）：

```
a       = 0000 0011
b       = 0000 0111
a ^ b   = 0000 0100
-(a<b)  = 1111 1111   (a < b 為真)
&       = 0000 0100
b ^     = 0000 0011   → 結果 = a = 3 ✓
```

硬體優勢：
- 編譯器將 `-(a < b)` 轉成 `cmp` + `sbb`，全程無分支
- 指令數從 v1 的 ~12 條降到 ~4–5 條
- SIMD / CUDA 環境下無多路徑 divergence，warp 效率高

---

## 結論

| | v1 (sol.c) | v2 |
|---|---|---|
| 比較運算子 | 不使用 | 使用 `<` / `>` |
| 指令數 | ~12+ | ~4–5 |
| CUDA / SIMD 適性 | 差 | 佳 |
| 學習價值 | 理解 sign bit 模擬比較 | 實戰首選寫法 |

sol.c 是理解底層的練習；v2 是實戰首選。
