# [INTERMEDIATE] 題目 04：計算 Set Bits 數量（Popcount）

## 題目描述
測試git

實作函式 `int popcount(unsigned int n)`，計算 `n` 的二進位表示中有多少個 `1`。

**禁止使用任何內建函式**，鼓勵使用 **Divide & Conquer 位元平行加總** 技巧。

---

## 函式簽名

```c
int popcount(unsigned int n);
```

---

## 測試案例

| 輸入         | 輸出 | 說明                     |
|--------------|------|--------------------------|
| `0`          | `0`  | 0b00...00，沒有 1        |
| `1`          | `1`  | 0b00...01，一個 1        |
| `7`          | `3`  | 0b0111，三個 1           |
| `255`        | `8`  | 0b11111111，八個 1       |
| `3735928559` | `24` | 0xDEADBEEF               |
| `4294967295` | `32` | 0xFFFFFFFF，全部 32 個 1 |

---

## 提示

有兩種主要思路：

- **方法 A（基礎迴圈）**：利用 `n & (n-1)` 每次清除最低位的 `1`，計數直到 n 為 0
- **方法 B（平行加總，挑戰目標）**：用 mask 將 32 bits 分組，平行加總每組的 1 的數量：
  ```
  mask1 = 0x55555555  → 每 2 bits 為一組
  mask2 = 0x33333333  → 每 4 bits 為一組
  mask3 = 0x0F0F0F0F  → 每 8 bits 為一組
  ```

---

## 限制

| 項目 | 規定 |
|------|------|
| 禁止使用 | `__builtin_popcount`、`_mm_popcnt_u32` 等內建函式 |
| 禁止使用 | 迴圈內的 `if/else` |
| 允許使用 | `&`、`\|`、`^`、`~`、`<<`、`>>` 及迴圈 |
| 挑戰目標 | 完全不使用迴圈，O(1) 純位元運算（方法 B） |
| 輸入範圍 | `0 ≤ n ≤ 4294967295`（32-bit unsigned int）|

---

## 實際應用場景

本題技巧在以下領域廣泛應用：

- **網路封包處理**：計算 IP checksum、統計封包中 flag bits 數量
- **資料庫引擎**：Bitmap Index 中快速統計符合條件的資料筆數（如 PostgreSQL `pg_popcount`）
- **GPU 運算（CUDA）**：`__popc(x)` 指令在 warp 層級做平行 popcount，用於稀疏矩陣運算
- **密碼學**：Hamming Distance 計算，用於模糊雜湊（fuzzy hashing）比對相似度
- **硬體設計**：CPU 的 `POPCNT` 指令（x86 SSE4.2）正是方法 B 的硬體實作

---

## 程式碼框架

```c
#include <stdio.h>

int popcount(unsigned int n) {
    // TODO: 使用位元運算實作
}

int main(void) {
    unsigned int n;
    while (scanf("%u", &n) == 1) {
        printf("%d\n", popcount(n));
    }
    return 0;
}
```

---

## 提交方式

完成後將你的 `.c` 檔放在此資料夾，並請 Claude 批改。
批改時會依照 `example_input.txt` → `example_output.txt` 驗證結果。
