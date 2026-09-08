# MyWave 项目 VAD（语音端点检测）算法说明

> 代码位置：`MyWave4_4_VS2010_0618_1057/MyWave/`
> 触发入口：菜单命令 `ID_POWER`（ID = 32825）→ `CMyWaveView::OnPower()`
> 涉及源码文件：
> - `MyWaveView.cpp`（算法主流程，~2704–2970 行）
> - `Speech.cpp` / `Speech.h`（`Frame`、`GetEnergy`、`GetZero`、`AddWindow` 等静态特征提取函数）

---

## 1. 算法总览

本项目中的 VAD 是一种**经典的基于“短时能量 + 短时过零率”的双门限端点检测算法**。
其基本思想是：

1. 把整段语音分帧（默认帧长 256，可选 512 / 1024），帧移 50%。
2. 计算每一帧的**短时能量**与**短时过零率（ZCR）**两个参数。
3. 根据能量最大值与一组相对门限，划出“语音段”的粗略起止点。
4. 再用过零率门限对边界进行**向左/向右扩展**。
5. 剔除短时噪声段、合并相邻段，得到最终端点列表。

整个流程只依赖两路时域特征，不涉及频谱、倒谱或统计模型（虽然工程里也有 HMM、GMM、MFCC 等代码，但端点检测本身并未用到）。

---

## 2. 预处理：分帧

在 `OnPower()` 中：

```cpp
unsigned int num = pDoc->data_one.size();                                  // 原始采样点数
int nCount = (int)floor((double)((num - m_nFrameSize) / (m_nFrameSize/2)) + 1); // 帧数
unsigned int sample_rate = nCount * m_nFrameSize;                          // 用于补零的总长度
CSpeech::Frame(m_nFrameSize, data, num, pDataOut);                         // 重叠分帧输出
```

- 帧长 `m_nFrameSize`：由菜单 `OnFrmlen256/512/1024` 设置，默认 256。
- 帧移：固定为帧长的 1/2（重叠 50%）。
- `CSpeech::Frame()`（Speech.cpp:91–118）的行为：直接拷贝采样到输出缓冲，不做预加重也不加窗；每帧 `nWinSize` 个样点，帧间步进 `nWinSize/2`。
- 与之相对的 `CSpeech::AddWindow()`（带预加重 + Hamming 加窗）在端点检测路径里没有被调用，所以本算法用的是**未加窗的原始信号**。

---

## 3. 特征 1：短时能量

调用 `CSpeech::GetEnergy(m_nFrameSize, pDataOut, sample_rate, dataout)`，
实现在 Speech.cpp:162–183：

```cpp
for (i = 0; i < nOutLen; i++) {           // nOutLen = sample_rate / nWinSize
    for (j = 0; j < nWinSize; j++)
        temp[j] = pDataIn[i*nWinSize + j];
    for (j = 0; j < nWinSize; j++)
        pDataOut[i] += temp[j] * temp[j]; // 每帧能量 = Σ x²
}
```

返回的是**未归一化的平方和**。

回到 `OnPower()`，再进行归一化与对数化：

```cpp
for (i = 0; i < nCount; i++)                       // 先除以帧长，得到平均能量
    dataout[i] = dataout[i] / m_nFrameSize;

for (i = 0; i < nCount; i++) {
    if (dataout[i] != 0)
        dataout[i] = 10 * log10(dataout[i]);       // 转换为 dB
    // 同时求出能量最大值 max、最小值 min
}
double energyLevel1 = max + deltaEnergyLevel1;     // deltaEnergyLevel1 = -20
double energyLevel2 = max + deltaEnergyLevel2;     // deltaEnergyLevel2 = -10
```

- **energyLevel2**（较高门限）：用于“粗定位”语音段。
- **energyLevel1**（较低门限）：用于把每段向左右扩展。

---

## 4. 特征 2：短时过零率（ZCR）

调用 `CSpeech::GetZero(m_nFrameSize, pDataOut, sample_rate, zcr)`，
实现在 Speech.cpp:137–159：

```cpp
for (i = 0; i < nOutLen; i++) {
    for (j = 0; j < nWinSize; j++)
        temp[j] = pDataIn[i*nWinSize + j];
    for (j = 0; j < nWinSize-1; j++)
        pDataOut[i] += abs(Sgn(temp[j]) - Sgn(temp[j+1])); // Sgn 为符号函数
    pDataOut[i] = pDataOut[i] / (2 * nOutLen);              // 归一化（注意这里的归一化是有 bug 的，详见 §8）
}
```

`Sgn` 在 Speech.h 中定义：`>=0 返回 1，<0 返回 -1`。因此 ZCR 计的是相邻样点符号位变化的次数的一半。

过零率门限：

```cpp
double max_zero = max(zcr);
double zcrRatio  = 0.20;
double zcrThreshold = max_zero * zcrRatio;
```

---

## 5. 端点检测主流程（核心步骤）

`OnPower()` 的核心是一个**由粗到精、再去噪**的 6 步流程：

### Step 1 — 高能量门限初筛（粗定位）

```cpp
std::vector<short> voiceIndex;
for (i = 0; i < nCount; i++)
    if (dataout[i] > energyLevel2)
        voiceIndex.push_back(i);
```

`voiceIndex` 存放所有能量超过 `energyLevel2`（= max − 10 dB）的帧下标。

### Step 2 — 帧间合并，得到连续段

```cpp
std::vector<short> sound;
sound.push_back(voiceIndex[0]);
for (i = 1; i < nums-1; i++) {
    if (voiceIndex[i+1] - voiceIndex[i] > 1) {   // 当前帧与下一帧不相邻，视为段结束/开始
        sound.push_back(voiceIndex[i]);           // 上一段尾
        sound.push_back(voiceIndex[i+1]);         // 下一段头
        k++;
    }
}
sound.push_back(voiceIndex[nums-1]);
```

把首尾依次成对压入 `sound`，得到若干个 `[start, end]` 段。

### Step 3 — 剔除短段（噪声小段）

```cpp
std::vector<short> sound2;
for (i = 0; i < sound.size()/2; i++)
    if (sound[2*i+1] - sound[2*i] >= 3)          // 段长 ≥ 3 帧才保留
        sound2.push_back(sound[2*i]),
        sound2.push_back(sound[2*i+1]);
```

### Step 4 — 用低能量门限向两端扩展

```cpp
for (i = 0; i < sound2.size()/2; i++) {
    head = sound2[2*i];
    while ((head-1) >= 0 && dataout[head-1] > energyLevel1)   // 向前扩展
        head--;
    sound2[2*i] = head;
    tail = sound2[2*i+1];
    while ((tail+1) < nCount && dataout[tail+1] > energyLevel1) // 向后扩展
        tail++;
    sound2[2*i+1] = tail;
}
```

这样能避免把语音头尾的弱辅音/弱清音误判为静音。

### Step 5 — 用 ZCR 门限再次向两端扩展

```cpp
for (i = 0; i < sound2.size()/2; i++) {
    head = sound2[2*i];
    while ((head-1) >= 0 && zcr[head-1] >= zcrThreshold)       // 用 ZCR 拉边界
        head--;
    sound2[2*i] = head;
    tail = sound2[2*i+1];
    while ((tail+1) < nCount && zcr[tail+1] > zcrThreshold)
        tail++;
    sound2[2*i+1] = tail;
}
```

清辅音（/s/, /f/, /t/ 等）能量低但过零率高，这一步专门用来把它们纳入端点内。

### Step 6 — 去除重复段

```cpp
std::vector<short> index, sound3;
for (i = 0; i < sound2.size()/2 - 1; i++)
    if (sound2[2*i]   == sound2[2*(i+1)] &&
        sound2[2*i+1] == sound2[2*(i+1)+1])
        index.push_back(i);
// 用 index 标记重复段，把不重复的帧对拷到 sound3
```

最后把 `sound3` 写 `energy3.txt`（每行两个下标：起点帧、终点帧），并把对应原始样本拷贝到 `datas` 用于输出/重保存。

---

## 6. 关键参数一览

| 参数 | 值 | 含义 |
| --- | --- | --- |
| `m_nFrameSize` | 256 / 512 / 1024（菜单可设） | 帧长 |
| 帧移 | `m_nFrameSize / 2` | 50% 重叠 |
| `deltaEnergyLevel1` | −20 dB | 低能量门限（用于边界扩展） |
| `deltaEnergyLevel2` | −10 dB | 高能量门限（用于粗筛语音段） |
| `energyLevel1` | `max + (-20)` | 实际低门限（dB） |
| `energyLevel2` | `max + (-10)` | 实际高门限（dB） |
| `zcrRatio` | 0.20 | ZCR 门限比例 |
| `zcrThreshold` | `max_zero × 0.20` | 实际 ZCR 门限 |
| 最短段长 | 3 帧 | 段长度阈值，短于它的不视作语音 |

---

## 7. 算法流程图

```
原始波形 data_one
        │
        ▼
  CSpeech::Frame           分帧（重叠 50%，未加窗、未预加重）
        │
        ├──► CSpeech::GetEnergy   ─► 每帧能量 E[i]
        │                                  │
        │                                  ▼
        │                       E[i] /= N ;  E[i] = 10·log10(E[i])
        │                                  │
        │                                  ▼
        │                         max = max(E)
        │              energyLevel1 = max − 20
        │              energyLevel2 = max − 10
        │
        └──► CSpeech::GetZero     ─► 每帧过零率 Z[i]
                                          │
                                          ▼
                                 max_zero = max(Z)
                                  Z_th = 0.20 · max_zero

       ┌──────────────────────────────────────────────┐
       │ Step 1：E[i] > energyLevel2  → voiceIndex    │
       │ Step 2：合并连续帧 → sound[start,end] 列表    │
       │ Step 3：剔除长度 < 3 帧的段 → sound2          │
       │ Step 4：低门限向左右扩展 → sound2              │
       │ Step 5：ZCR ≥ Z_th 向左右扩展 → sound2         │
       │ Step 6：去重  → sound3（最终端点）             │
       └──────────────────────────────────────────────┘
                                          │
                                          ▼
                       对应原始样本拷出，写 energy3.txt
```

---

## 8. 代码中可注意的几个小问题（仅作记录）

1. **`GetZero` 归一化分母偏大**：
   实际过零率定义应为 `ZCR = (符号变化次数) / (2·N)`，其中 N 是帧长。
   代码里写的是 `pDataOut[i] = pDataOut[i] / (2 * nOutLen)`，`nOutLen` 是**总帧数**而非帧长，会让 ZCR 整体远小于真实值，但因为后面门限是按当前 `max_zero` 自适应取的，**算法功能不受影响**。

2. **Step 6 的去重判断**：
   `if (sound2[2*i] == sound2[2*(i+1)] && sound2[2*i+1] == sound2[2*(i+1)+1])` 只比较相邻两段完全相同的情况，**无法去除重叠段**（如果 Step 4/5 把两段扩到相连甚至重叠，仍会一起输出）。

3. **Step 1 的 `voiceIndex` 边界**：
   循环中没有判断 `nums == 0` 的情况，若整段都不超过高门限会越界访问 `voiceIndex[0]`。实际工程上通常假设至少有一帧超出门限。

4. **没用 `AddWindow`（未加窗、未预加重）**：
   端点检测路径里调用的是 `CSpeech::Frame`（不加窗），与 `AddWindow`（带预加重 + Hamming）的特征在数值含义上不同，但在本算法下也够用。

5. **头文件 `Speech.h` 中还有 `THRESHOLD_ENERGY = 30` 这个宏**，但本 VAD 路径里并未引用，门限全部是相对 `max` 计算得到的自适应门限。

---

## 9. 小结

这套 VAD 是教科书式的“两级能量门限 + 过零率修正”方案：

- **粗筛**：用相对 `max` 的高能量门限（−10 dB）挑出明显是语音的帧。
- **扩展**：用低能量门限（−20 dB）和 ZCR 门限（0.20·max）把每段向两端延伸，把清音/弱辅音也包进来。
- **净化**：丢掉长度小于 3 帧的小段，去掉完全重复的相邻段。

优点是实现简单、自适应（门限相对最大值），不依赖训练数据；
缺点是对噪声平稳度比较敏感，且不区分静音/噪声，对强突发噪声（如关门声）容易误触发。

---

## 10. 常见疑问：本算法用没用到 FFT / 频谱 / 倒谱 / MFCC？

**答：完全没有。** 这是纯时域的端点检测算法。

证据：在 `OnPower()` 的函数体内用 `grep -iE "fft|fourier|spectrum|cepstrum|mfcc|mel|lpc|filter"` 检索，**匹配数 = 0**。
它只调用了三个 `CSpeech` 静态方法，且都是时域操作：

| 调用 | 实现位置 | 涉及域 |
| --- | --- | --- |
| `CSpeech::Frame` | `Speech.cpp:91` | 时域，仅做重叠分帧（不加窗、不预加重） |
| `CSpeech::GetEnergy` | `Speech.cpp:162` | 时域，`Σ x²` |
| `CSpeech::GetZero` | `Speech.cpp:137` | 时域，符号位变化计数 |

项目里**确实**存在大量频域/倒谱代码（`Methods.cpp::FFT/QFFT/IFFT/QIFFT`、`CWaveConvertor::ConvertToFFT`、`CSpeech::GetMFCC/GetMelSpectrum/GetLPC/GetLPCC` 等），但它们挂在**别的菜单命令**上，跟 VAD 这条路径互不调用。如下表所示：

| 菜单命令 | 处理函数 | 是否被 VAD 调用 |
| --- | --- | --- |
| `ID_FFT` | `OnFft()` | ❌（独立：调 `CWaveConvertor::ConvertToFFT`） |
| `ID_SPECTRUM` | `OnSpectrum()` | ❌ |
| `ID_LOGSPECTRUM` | `OnLogspectrum()` | ❌ |
| `ID_CEPSTRUM` | `OnCepstrum()` | ❌ |
| `ID_MFCC` | `OnMfcc()` | ❌ |
| `ID_LPCTODATA` | `OnLpctodata()` | ❌ |
| `ID_CEP_Pitch` | `OnCepPitch()` | ❌ |
| `ID_TwiceFFT` / `ID_TwiceFFT2` | `OnTwicefft()/OnTwicefft2()` | ❌ |
| `ID_POWER` | **`OnPower()`** | ✅ 这就是 VAD 本体 |
| `ID_ZERO` | `OnZero()` | ❌（仅显示过零率曲线） |
| `ID_VIBRATION` | `OnVibration()` | ❌（仅显示平均幅度） |

所以这份 VAD 是“教科书式”的纯时域实现；工程里那些 FFT / MFCC / LPC 模块是为**基音检测、频谱/倒谱可视化、语音合成**等其它功能准备的，**端点检测这一段没有用上**。如果将来想升级，可以考虑：

1. 在 `Frame` 后加一步 `CSpeech::AddWindow`（带预加重 + Hamming），让能量与过零率更稳定；
2. 引入**频谱平坦度 / 子带能量比**等频域特征来增强对平稳噪声的鲁棒性；
3. 把 `Methods.cpp` 里现成的 `Methods::FFT / QFFT` 直接复用作前置 FFT 通路。
