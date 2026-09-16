# UEFI-Firmware-Practice

## 關於這個專案
這是紀錄我大學期間接觸 UEFI/BIOS 韌體開發的練習專案。裡面放的主要是我在培訓與實習期間寫的底層 C 語言原始碼。

比起寫上層的應用程式，寫 BIOS 真的滿不一樣的，常常要自己啃硬體規格書 (Spec)，小心翼翼地操作記憶體跟 I/O Port，一不小心指標踩雷系統就直接死機。這些實作練習讓我對 X86 系統底層的開機流程有了比較具體的概念。

## 目錄結構與內容

專案依據實作的主題與 UEFI 的不同階段，分類如下：

### 1. 系統管理模式 (SMM)
*   **`SMM_Homework/`** 
    *   這部分是在練習怎麼寫 SMI Handler[cite: 6]。SMM 是 X86 架構裡權限很高的模式，這支程式主要是練習當硬體中斷觸發時，系統在底層是怎麼接管並處理的。

### 2. PEI 階段初始化
*   **`PEI_PPI/`** (包含 `TestppiA`, `TestppiB` 模組[cite: 6])
    *   這是在 BIOS 開機很前期的階段（記憶體可能都還沒完全準備好）[cite: 6]。
    *   這裡主要練習寫 PEIM 模組，並且用 PPI (PEIM-to-PEIM Interface) 介面來互相傳遞資料[cite: 6]。

### 3. 硬體控制器存取 (Hardware Access)
這區塊都是直接用 C 語言操作 I/O Port 或是暫存器來跟硬體溝通的練習：
*   **`CMOS/`**：練習透過 Index/Data Port 去讀寫 CMOS 裡面的資料[cite: 6]。
*   **`KBC/`**：直接對 8042 鍵盤控制器 (Keyboard Controller) 下 Command[cite: 6]。
*   **`SPD/`**：透過 SMBus 去抓取記憶體 (RAM) 上的 SPD 參數[cite: 6]。
*   **`IRQ/`**：中斷請求 (Interrupt Request) 的配置與追蹤練習[cite: 6]。

### 4. UEFI 核心協定應用
*   **`Handle_Protocol/`**：練習 UEFI 最核心的 Handle Protocol，熟悉模組之間是怎麼透過介面互相溝通的[cite: 6]。
*   **`Variable/`**：練習讀寫 NVRAM 裡面的 UEFI 變數 (Environment Variables)[cite: 6]。
*   **`CPU_AP/`**：處理器相關的基礎配置與狀態讀取練習[cite: 6]。

## 實作心得與收穫
*   **啃 Spec 的耐心**：底層開發很多時候沒有現成的 API 可以呼叫，只能乖乖翻原廠的 Datasheet，慢慢找 Register 對應的位址跟 Bit 定義。
*   **C 語言底層手感**：對指標操作、記憶體邊界跟 Bitwise 運算變得非常敏感，因為只要沒寫好，通常就是直接 Hang 住，沒有例外訊息可以看。
*   **純手工 Debug 訓練**：寫韌體常常不能像寫軟體那樣輕鬆下中斷點，只能靠觀察記憶體值或 Register 的狀態來推敲 Bug 在哪裡，非常磨練除錯的邏輯跟抗壓性。
