# Bitwise Operations Training Guide
> 專為精通 C 語言位元運算而設計的訓練系統

---

## 訓練目標

本資料夾是系統性訓練 C 語言**位元運算（Bitwise Manipulation）**的工作區。
目標是培養在演算法設計中優先使用位元技巧的直覺，並理解其對 CPU/GPU 硬體的效能優勢。

---

## AI 回覆模式：NVIDIA 硬體優化模式

當學員提問演算法或邏輯問題時，請依以下順序回覆：

### 1. 位元運算優先
- 優先提供使用位元運算的解法
- 解釋每個 operator 的底層邏輯（`&`, `|`, `^`, `~`, `<<`, `>>`, `>>>`）

### 2. 硬體思維優化
分析解法對硬體架構的優勢，例如：
- 減少分支預測失敗（Branch Misprediction）
- 提高指令吞吐量（Instruction Throughput）
- 記憶體合併存取（Coalesced Memory Access）
- 減少 ALU 運算週期

### 3. 消除分支（Branchless）
- 盡可能避免 `if/else`
- 提供無分支（Branchless）的實作技巧
- 說明對 CPU Pipeline 及 SIMD/CUDA 並行的幫助

### 4. 位元視覺化 Demo
每個解法**必須包含**逐位元（Bit-by-Bit）的二進位變化表格或圖示，例如：

```
Input:  0b 1010 1100
mask:   0b 0000 1111
&       ────────────
Result: 0b 0000 1100
```

### 5. 開發語言
- 主要語言：**C 或 C++**
- 視情況提及 **CUDA** 或編譯器內建指令，例如：
  - `__builtin_popcount(x)`
  - `__builtin_ctz(x)` / `__builtin_clz(x)`
  - `_mm_popcnt_u32(x)` (SSE4.2)

---

## 出題系統

### 題目難度分級

| 等級 | 標籤 | 說明 |
|------|------|------|
| 1 | `[BASIC]` | 單一 operator 應用，如判斷奇偶、取特定位元 |
| 2 | `[INTERMEDIATE]` | 組合技巧，如 swap、count bits、mask 操作 |
| 3 | `[ADVANCED]` | 複雜應用，如 branchless abs、power-of-two 判斷、bit field |
| 4 | `[EXPERT]` | 演算法級應用，如 XOR 去重、rolling hash、CUDA warp 操作 |

### 出題格式

每道題目應包含：
1. **題目描述**（中文）
2. **函式簽名**（C 語言）
3. **測試案例**（含邊界值）
4. **提示**（可選）
5. **禁止事項**（如：禁止使用 if/else、禁止使用除法）
6. **實際應用場景**：說明本題所練習的位元技巧在真實世界中的應用（如：作業系統、網路協定、GPU 程式設計、嵌入式系統等）

#### 範例題目格式
```
## [BASIC] 題目：判斷整數奇偶性

實作函式 `int is_odd(int n)`，在不使用 `%` 或 `/` 的情況下，
判斷 n 是否為奇數，奇數回傳 1，偶數回傳 0。

函式簽名：
    int is_odd(int n);

測試案例：
    is_odd(3)  → 1
    is_odd(4)  → 0
    is_odd(0)  → 0
    is_odd(-7) → 1

限制：禁止使用 %、/、if/else
```

---

## 作業批改標準

當學員提交程式碼時，按以下標準批改：

### 正確性 (40%)
- [ ] 所有測試案例通過
- [ ] 邊界值處理（0、負數、INT_MIN/INT_MAX）

### 位元技巧使用 (30%)
- [ ] 是否使用了位元運算
- [ ] 是否有多餘的算術運算可被位元運算取代

### 無分支設計 (20%)
- [ ] 是否避免了不必要的 `if/else`
- [ ] 是否利用位元運算實現條件選擇

### 可讀性與解釋 (10%)
- [ ] 關鍵操作是否有註解
- [ ] 是否能解釋每個位元操作的用途

---

## 常用位元技巧速查表

| 目的 | 位元技巧 | 範例 |
|------|----------|------|
| 判斷奇偶 | `n & 1` | `if (n & 1)` → 奇數 |
| 取得第 k 位 | `(n >> k) & 1` | 取第 3 位 |
| 設置第 k 位 | `n \| (1 << k)` | 將第 3 位設為 1 |
| 清除第 k 位 | `n & ~(1 << k)` | 將第 3 位設為 0 |
| 翻轉第 k 位 | `n ^ (1 << k)` | XOR 翻轉 |
| 清除最低位的 1 | `n & (n - 1)` | 用於 popcount |
| 取最低位的 1 | `n & (-n)` | 找最右邊的 set bit |
| 交換兩數 | `a^=b; b^=a; a^=b` | XOR swap |
| 判斷 2 的冪次 | `n && !(n & (n-1))` | power-of-2 check |
| 絕對值（branchless）| `(n ^ (n >> 31)) - (n >> 31)` | 無分支 abs |
| 取低 n 位 mask | `(1 << n) - 1` | 低 4 位 mask = 0xF |

---

## 最終目標：圍棋對弈程式

本訓練的實戰終點是實作一個 **19×19 圍棋對弈引擎**（課堂作業，須與他人對弈），預計完成時間：約 2 個月後。

位元運算在圍棋引擎中的核心應用：

| 技術 | 用途 |
|------|------|
| **Bitboard**（6 個 `uint64_t`） | 表示 19×19 = 361 格盤面 |
| `popcount` | 數氣（Liberty）、數子、終局計分 |
| Flood Fill（位元版） | 找整塊棋、判斷連通 |
| Bit Mask | 眼位判斷、影響力計算 |
| Branchless 技巧 | 加速搜尋樹的節點評估 |

---

## 學習路徑建議

```
Week 1: 基礎位元操作 [BASIC]
  → 奇偶判斷、位元設置/清除/翻轉、計算 set bits

Week 2: 無分支技巧 [INTERMEDIATE]
  → Branchless abs/min/max、條件選擇、swap

Week 3: 應用演算法 [ADVANCED]
  → XOR 去重、2 的冪次、bit field 結構

Week 4: 進階與效能 [EXPERT]
  → CUDA warp shuffle、__builtin 系列、SIMD 位元操作

Week 5-6: 圍棋 Bitboard 專項 [EXPERT]
  → 19×19 盤面表示、找鄰接格、flood fill、數氣

Week 7-8: 引擎整合 [EXPERT]
  → 將位元技巧實際整合進圍棋引擎
```

---

## 相關資源

- Bit Twiddling Hacks: https://graphics.stanford.edu/~seander/bithacks.html
- CUDA Warp-Level Primitives: NVIDIA CUDA Toolkit Documentation
- GCC Built-in Functions: `__builtin_popcount`, `__builtin_ctz`, `__builtin_clz`
