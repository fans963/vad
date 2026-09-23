# MyWaveQt

基于 C++23 与 Qt 6 开发的桌面语音信号分析与端点检测系统，面向语音信号处理的教学、科研训练与算法原型验证。

## 主要功能

- **音频与波形展示**：支持 WAV、MP3、FLAC、OGG 等格式的解码与整段平滑回放，支持多声道波形显示、缩放平移与明暗主题切换；
- **时域分析**：短时能量、短时平均振幅、短时过零率、修正自相关（ACF）、AMDF 基音周期检测；
- **频域分析**：幅度谱/功率谱、真实倒谱、Mel 频谱、13 维 MFCC、LPC/LPCC、二维语谱图；
- **端点检测 (VAD)**：集成双门限 VAD、能量 VAD、过零率 VAD 等算法，提供能量与过零率门限调试轨道和有效语音段标记。

## 快速开始

### 1. 依赖与环境

- 支持 C++23 的编译器（GCC 13+ / Clang 17+ / MSVC 2022）
- CMake 3.20+ 与 Ninja
- 系统 Qt 6 基础库（Core、Gui、Widgets、PrintSupport、Concurrent、OpenGLWidgets）
- Git（用于拉取三方子模块）

以 Arch Linux 为例：
```bash
sudo pacman -S --needed cmake ninja gcc qt6-base qt6-wayland
```

### 2. 克隆与初始化

```bash
git clone --recurse-submodules https://github.com/fans963/vad.git
cd vad

# 初始化内嵌的 vcpkg（用于管理 FFTW3、miniaudio、mimalloc）
./3rdparty/vcpkg/bootstrap-vcpkg.sh -disableMetrics
```

### 3. 构建与运行

Release 构建：
```bash
cmake --preset release
cmake --build --preset release
./build/MyWaveQt
```

运行单元测试：
```bash
ctest --test-dir build --output-on-failure
```

## 许可说明

本项目依赖及包含的三方组件遵循各自开源协议：QCustomPlot 2.1.1 (GPL-3.0-or-later)、miniaudio (MIT-0 / Public Domain)、mimalloc (MIT)、FFTW3 (GPL-2.0-or-later) 以及 Qt 6 (LGPL-3.0 / GPL-3.0)。
