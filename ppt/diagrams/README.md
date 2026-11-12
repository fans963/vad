# Excalidraw 图表素材

本目录包含两张用于结题答辩的可编辑架构/流程图：

| 文件 | 内容 | 画布 |
|:---|:---|:---:|
| `system-architecture.excalidraw` | MyWaveQt 四层架构与输入/输出数据流 | 1200 × 560 |
| `vad-pipeline.excalidraw` | VAD 五步处理流水线与参数/复核说明 | 1200 × 480 |

每张图同时提供：

- `.excalidraw`：可编辑源文件，直接拖入 [excalidraw.com](https://excalidraw.com) 或用 VS Code Excalidraw 插件打开。
- `.svg`：矢量导出，适合继续改色或排版。
- `.png`：2400 px 宽的高分辨率位图，已用于 PPT 附录页。

## 重新生成

```bash
cd ppt
node make_diagrams.js
for f in diagrams/*.svg; do
  rsvg-convert -w 2400 -o "${f%.svg}.png" "$f"
done
```

## 设计说明

- 图表使用与 PPT 一致的深海军蓝、青绿、琥珀色体系。
- `.excalidraw` 中的文字统一使用 `fontFamily: 5`（Excalifont），便于在 Excalidraw 内继续编辑。
- PNG 导出使用 `Noto Sans CJK SC` 渲染，保证中文在答辩投影上清晰；如果在本机重新导出，请确认系统已安装该字体或替换为 `Microsoft YaHei`。
- 为适配正式答辩，源文件采用 `roughness: 0` 的低粗糙度风格，而不是默认手绘风格。
