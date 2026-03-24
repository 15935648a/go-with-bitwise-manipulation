# [ADVANCED] 題目：RGB 色彩通道操作

螢幕色彩常以 32 位元整數儲存，格式為 `0x00RRGGBB`：

```
Bit 31-24 : 未使用 (0x00)
Bit 23-16 : R (紅色, 0~255)
Bit 15-8  : G (綠色, 0~255)
Bit 7-0   : B (藍色, 0~255)
```

實作以下三個函式：

```c
// 從 color 中取出 R 值
int get_r(unsigned int color);

// 從 color 中取出 G 值
int get_g(unsigned int color);

// 從 color 中取出 B 值
int get_b(unsigned int color);

// 將 r, g, b 三個值打包成 0x00RRGGBB 格式
unsigned int pack_rgb(int r, int g, int b);
```

## 測試案例

```
color = 0xFF8040

get_r(color) → 255   (0xFF)
get_g(color) → 128   (0x80)
get_b(color) → 64    (0x40)

pack_rgb(255, 128, 64) → 0x00FF8040

pack_rgb(0, 0, 0)      → 0x00000000
pack_rgb(255, 255, 255)→ 0x00FFFFFF
```

## 限制

- 禁止使用 `if/else`
- 只能使用 `&`、`|`、`<<`、`>>` 完成所有操作
- 禁止使用乘除法

## 提示

兩個核心動作：

**提取**：先 shift 讓目標欄位移到最低位，再用 mask 取出
```
想取 bit 23-16 → 先 >> 16，再 & 0xFF
```

**打包**：先 shift 每個值到正確位置，再用 | 合併
```
R 要放在 bit 23-16 → R << 16
```

## 實際應用場景

| 場景 | 用途 |
|------|------|
| **GPU/圖形渲染** | RGBA 通道操作是 shader 最基礎的位元技巧 |
| **網路協定** | IP header、TCP flags 都用 bit field 壓縮欄位 |
| **圍棋 Bitboard** | 每個格子的狀態（空/黑/白）可用 2 bits 表示，整塊盤面壓進數個 uint64 |
| **嵌入式系統** | 暫存器操作（GPIO、SPI、I2C）完全依賴 bit field 讀寫 |
| **檔案格式** | PNG、BMP、ELF 等格式 header 大量使用 bit field |
