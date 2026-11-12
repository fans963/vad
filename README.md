# MyWaveQt

基于现代 C++23 与 Qt 6 开发的高性能桌面语音信号分析与端点检测系统，面向数字语音处理的教学、科研训练与声学算法验证。

---

## 🌟 主要功能

- **音频与波形展示**：基于 miniaudio 原生解码 WAV、MP3、FLAC、OGG 等格式，支持整段音频低延迟平滑回放、多声道波形显示与深浅色主题切换；
- **时域声学分析**：短时能量、短时平均振幅、短时过零率、自相关（ACF）与平均幅度差（AMDF）基音周期检测；
- **频域与倒谱分析**：短时傅里叶变换（STFT）幅度谱/功率谱、真实倒谱（Real Cepstrum）、Mel 频谱、13 维 MFCC、线性预测（LPC/LPCC）与二维语谱图；
- **语音端点检测 (VAD)**：集成经典 Rabiner-Sambur 能量与过零率双门限 VAD、Silero VAD（基于精简 OpenVINO CPU 运行时推理）、倒谱距离 VAD 及单特征检测器，提供门限调试辅助轨道与有效语音段区间标记；
- **多泳道视口与交互优化**：OpenGL 硬件加速多泳道归一化层叠展示、自适应并行极值 LOD 降采样、零开销悬浮坐标指示与防重叠智能坐标标尺。

---

## 🏗️ 系统架构

系统遵循分层解耦的模块化设计，划分为四个核心层次：

[![系统总体架构](report/figures/architecture.png)](report/figures/architecture.svg)

- **用户界面展示层 (UI)**：基于 QCustomPlot 与 OpenGL 实现多声道波形与高维特征的多泳道层叠展示；
- **端点检测调度层 (VAD)**：外观门面 `VadEngine` 统一调度双门限与各单项检测器；
- **数字信号处理核心层 (DSP)**：无外部 UI 依赖的数学计算核心，涵盖分帧加窗、FFT 规划缓存、倒谱与特征抽取；
- **核心基础与系统服务层 (Core)**：基于 miniaudio 的极简音频解码与低延迟回调推流播放，集成多级图表特征缓存。

---

## ⚡ 全链路高性能重构

针对大规模特征计算与千万级采样点长音频，系统推行了全链路软硬件协同优化：

[![DSP 流水线与内存模型对比](report/figures/dsp_pipeline.png)](report/figures/dsp_pipeline.svg)

- **零碎片连续内存**：采用 `FramedSignal` 单块连续物理内存替代离散嵌套向量，全链路裸指针跨度切片，从根源上杜绝堆内存碎片，极大提升 CPU L1/L2 缓存行命中率；
- **FFTW 规划缓存池**：设计线程安全的单例 `PlanCache`，杜绝重复创建 Plan 造成的锁竞争与决策开销；
- **粗粒度多线程与异步解耦**：采用区间分块 `parallelFor` 消除微任务调度开销，后台计算通过 QtConcurrent 异步派发，保障交互界面永久 60 FPS 满帧刷新。

---

## 🔍 经典双门限语音端点检测 (VAD)

系统实现了经典的 Rabiner-Sambur 能量与过零率四级状态机判决算法：

[![双门限 VAD 流程图](report/figures/vad_flowchart.png)](report/figures/vad_flowchart.svg)

1. **核心发音段确立**：短时能量持续超过高门限 $E_H$ 达到 $N_v$ 帧，精准过滤孤立爆破噪声；
2. **能量低门限延展**：依据低门限 $E_L$ 向前向后回溯，锁定元音主体发音边界；
3. **过零率二次拓展**：利用过零率门限 $Z_{\text{th}}$ 沿边界向外搜寻，挽救高频弱能量清辅音；
4. **间隙平滑合并**：自动合并短于 $N_{\text{gap}}$ 的短暂发音停顿（如字词闭塞音），消除内部断裂。

---

## 🚀 快速开始

### 1. 编译与环境依赖

- 支持 C++23 的编译器（GCC 13+ / Clang 17+ / MSVC 2022）
- CMake 3.20+ 与 Ninja
- 系统 Qt 6 基础库（Core、Gui、Widgets、PrintSupport、Concurrent、OpenGLWidgets）
- Git（用于拉取三方子模块）

以 Arch Linux / Ubuntu 为例：
```bash
# Arch Linux
sudo pacman -S --needed cmake ninja gcc qt6-base qt6-wayland

# Ubuntu 24.04
sudo apt update && sudo apt install -y cmake ninja-build build-essential qt6-base-dev libgl1-mesa-dev
```

Windows 环境（以安装官方预编译 Qt 6 + MSVC 2022 为例）：
- 安装 Visual Studio 2022（勾选“使用 C++ 的桌面开发”）
- 安装官方预编译 Qt 6（例如安装路径为 `C:\Qt\6.7.2\msvc2022_64`）

### 2. 克隆与初始化

```bash
# 递归克隆代码仓库与三方子模块
git clone --recurse-submodules https://github.com/fans963/vad.git
cd vad

# 初始化内嵌的 vcpkg（用于自动化管理 miniaudio、FFTW3 与 OpenVINO）
# Linux / macOS
./3rdparty/vcpkg/bootstrap-vcpkg.sh -disableMetrics

# Windows (PowerShell)
.\3rdparty\vcpkg\bootstrap-vcpkg.bat -disableMetrics
```

### 3. 构建与运行

#### Linux / macOS
```bash
# 采用 CMake Release 预设进行极速构建
cmake --preset release
cmake --build --preset release -j $(nproc)

# 启动程序主界面
./build/MyWaveQt

# 运行 DSP 数值精度对齐测试套件
ctest --test-dir build --output-on-failure
```

#### Windows (使用预编译 Qt 6)
```powershell
# 1. 配置工程（将 CMAKE_PREFIX_PATH 指向您的 Qt 安装根目录）
cmake --preset windows-release -DCMAKE_PREFIX_PATH="C:/Qt/6.7.2/msvc2022_64"

# 2. 构建 Release 版本
cmake --build --preset windows-release --config Release

# 3. （可选）自动打包 Qt 运行时 DLL 至可执行文件目录
cmake --build --preset windows-release --target deploy_qt

# 4. 运行
.\build-windows\Release\MyWaveQt.exe
```

---

## 📄 许可说明

本项目依赖及包含的三方组件遵循各自开源协议：
- [QCustomPlot](https://www.qcustomplot.com/) 2.1.1 (GPL-3.0-or-later)
- [miniaudio](https://github.com/mackron/miniaudio) (MIT-0 / Public Domain)
- [mimalloc](https://github.com/microsoft/mimalloc) (MIT)
- [FFTW3](http://www.fftw.org/) (GPL-2.0-or-later)
- [Qt 6](https://www.qt.io/) (LGPL-3.0 / GPL-3.0)
