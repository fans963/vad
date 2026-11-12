const fs = require('fs');
const path = require('path');

const OUT = path.join(__dirname, 'diagrams');
fs.mkdirSync(OUT, { recursive: true });

const C = {
  ink: '#0B1220',
  navy: '#14315C',
  blue: '#2F5FE3',
  teal: '#0E7C75',
  amber: '#B45309',
  rose: '#B4233C',
  muted: '#5B6B82',
  line: '#DCE3EC',
  softBlue: '#EEF3FB',
  softTeal: '#E8F4F2',
  softAmber: '#FBF0E4',
  softGray: '#F1F5F9',
  white: '#FFFFFF',
};

const FONT_SVG = 'Noto Sans CJK SC, Microsoft YaHei, sans-serif';
const now = Date.now();

function esc(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

class Diagram {
  constructor(name, width, height, title, subtitle) {
    this.name = name;
    this.width = width;
    this.height = height;
    this.elements = [];
    this.svg = [];
    this.index = 0;
    this.seed = 1000;
    this.markerColors = new Set();

    this.svg.push(
      `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" viewBox="0 0 ${width} ${height}">`,
      `<defs>`,
    );
    this.markerColors.add(C.muted);
    this.markerColors.add(C.blue);
    this.markerColors.add(C.teal);
    this.markerColors.add(C.amber);
    this.markerColors.add(C.navy);
    for (const color of this.markerColors) {
      const id = `arrow-${color.replace('#', '')}`;
      this.svg.push(
        `<marker id="${id}" markerWidth="9" markerHeight="9" refX="8" refY="4.5" orient="auto" markerUnits="strokeWidth">` +
        `<path d="M0,0 L0,9 L9,4.5 z" fill="${color}"/></marker>`,
      );
    }
    this.svg.push(`</defs>`);
    this.svg.push(
      `<rect x="0" y="0" width="${width}" height="${height}" fill="#FFFFFF"/>`,
    );

    if (title) {
      this.text({
        x: 50, y: 38, text: title, size: 28, color: C.ink, bold: true, anchor: 'start', excY: 38,
      });
    }
    if (subtitle) {
      this.text({
        x: 50, y: 78, text: subtitle, size: 13, color: C.muted, anchor: 'start', excY: 78,
      });
    }
  }

  nextIndex() {
    return `a${this.index++}`;
  }

  nextSeed() {
    return (this.seed += 17);
  }

  addRect({ x, y, w, h, fill = C.white, stroke = C.line, strokeWidth = 1.5, rx = 14, opacity = 100 }) {
    const id = `${this.name}-r${this.index}`;
    this.elements.push({
      id,
      type: 'rectangle',
      x, y, width: w, height: h,
      angle: 0,
      strokeColor: stroke,
      backgroundColor: fill,
      fillStyle: 'solid',
      strokeWidth,
      strokeStyle: 'solid',
      roughness: 0,
      opacity,
      groupIds: [],
      frameId: null,
      index: this.nextIndex(),
      roundness: { type: 3 },
      seed: this.nextSeed(),
      version: 1,
      versionNonce: this.nextSeed(),
      isDeleted: false,
      boundElements: null,
      updated: now,
      link: null,
      locked: false,
    });
    this.svg.push(
      `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${rx}" fill="${fill}" ` +
      `stroke="${stroke}" stroke-width="${strokeWidth}" opacity="${opacity / 100}"/>`,
    );
    return id;
  }

  addLine({ x1, y1, x2, y2, color = C.line, width = 1, dashed = false, opacity = 1 }) {
    const id = `${this.name}-l${this.index}`;
    const dx = x2 - x1;
    const dy = y2 - y1;
    this.elements.push({
      id,
      type: 'line',
      x: x1,
      y: y1,
      width: Math.abs(dx),
      height: Math.abs(dy),
      angle: 0,
      strokeColor: color,
      backgroundColor: 'transparent',
      fillStyle: 'solid',
      strokeWidth: width,
      strokeStyle: dashed ? 'dashed' : 'solid',
      roughness: 0,
      opacity: opacity * 100,
      groupIds: [],
      frameId: null,
      index: this.nextIndex(),
      roundness: null,
      seed: this.nextSeed(),
      version: 1,
      versionNonce: this.nextSeed(),
      isDeleted: false,
      boundElements: null,
      updated: now,
      link: null,
      locked: false,
      points: [[0, 0], [dx, dy]],
      startBinding: null,
      endBinding: null,
      startArrowhead: null,
      endArrowhead: null,
    });
    this.svg.push(
      `<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${color}" ` +
      `stroke-width="${width}" opacity="${opacity}"${dashed ? ' stroke-dasharray="5 5"' : ''}/>`,
    );
    return id;
  }

  addArrow({ x1, y1, x2, y2, color = C.muted, width = 1.8, dashed = false, label = '', labelX = null, labelY = null }) {
    const id = `${this.name}-a${this.index}`;
    const dx = x2 - x1;
    const dy = y2 - y1;
    this.markerColors.add(color);
    this.elements.push({
      id,
      type: 'arrow',
      x: x1,
      y: y1,
      width: Math.abs(dx),
      height: Math.abs(dy),
      angle: 0,
      strokeColor: color,
      backgroundColor: 'transparent',
      fillStyle: 'solid',
      strokeWidth: width,
      strokeStyle: dashed ? 'dashed' : 'solid',
      roughness: 0,
      opacity: 100,
      groupIds: [],
      frameId: null,
      index: this.nextIndex(),
      roundness: { type: 2 },
      seed: this.nextSeed(),
      version: 1,
      versionNonce: this.nextSeed(),
      isDeleted: false,
      boundElements: null,
      updated: now,
      link: null,
      locked: false,
      points: [[0, 0], [dx, dy]],
      startBinding: null,
      endBinding: null,
      startArrowhead: null,
      endArrowhead: 'arrow',
    });
    this.svg.push(
      `<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${color}" ` +
      `stroke-width="${width}" marker-end="url(#arrow-${color.replace('#', '')})"` +
      `${dashed ? ' stroke-dasharray="5 5"' : ''}/>`,
    );
    if (label) {
      const lx = labelX === null ? (x1 + x2) / 2 : labelX;
      const ly = labelY === null ? (y1 + y2) / 2 - 10 : labelY;
      const n = String(label).length;
      const tw = Math.max(34, n * 7.2 + 10);
      this.svg.push(
        `<rect x="${lx - tw / 2}" y="${ly - 8}" width="${tw}" height="16" rx="8" fill="#FFFFFF" opacity="0.92"/>`,
      );
      this.addTextOnly({ cx: lx, cy: ly, text: label, size: 10.5, color: C.muted, anchor: 'middle' });
    }
    return id;
  }

  addTextOnly({ cx, cy, text, size = 14, color = C.ink, bold = false, anchor = 'middle', width = null }) {
    const w = width || Math.max(30, String(text).length * size * 0.9);
    const x = anchor === 'middle' ? cx - w / 2 : anchor === 'end' ? cx - w : cx;
    const id = `${this.name}-t${this.index}`;
    this.elements.push({
      id,
      type: 'text',
      x,
      y: cy - size * 0.58,
      width: w,
      height: size * 1.3,
      angle: 0,
      strokeColor: color,
      backgroundColor: 'transparent',
      fillStyle: 'solid',
      strokeWidth: 1,
      strokeStyle: 'solid',
      roughness: 0,
      opacity: 100,
      groupIds: [],
      frameId: null,
      index: this.nextIndex(),
      roundness: null,
      seed: this.nextSeed(),
      version: 1,
      versionNonce: this.nextSeed(),
      isDeleted: false,
      boundElements: null,
      updated: now,
      link: null,
      locked: false,
      text: String(text),
      originalText: String(text),
      fontSize: size,
      fontFamily: 5,
      textAlign: anchor === 'middle' ? 'center' : anchor === 'end' ? 'right' : 'left',
      verticalAlign: 'top',
      lineHeight: 1.25,
      baseline: Math.round(size * 0.8),
    });
    this.svg.push(
      `<text x="${cx}" y="${cy}" text-anchor="${anchor}" dominant-baseline="middle" ` +
      `font-family="${FONT_SVG}" font-size="${size}" ` +
      `font-weight="${bold ? 700 : 400}" fill="${color}">${esc(text)}</text>`,
    );
    return id;
  }

  text({ x, y, text, size = 14, color = C.ink, bold = false, anchor = 'start', excY = null }) {
    const svgY = y;
    const excYValue = excY === null ? y - size * 0.58 : excY;
    return this.addTextOnly({ cx: x, cy: svgY, text, size, color, bold, anchor, width: null, });
  }

  chip({ x, y, w, h, label, fill, stroke = null, color = C.ink, size = 13, bold = false, rx = 8 }) {
    this.addRect({ x, y, w, h, fill, stroke: stroke || fill, strokeWidth: 1, rx });
    this.addTextOnly({ cx: x + w / 2, cy: y + h / 2, text: label, size, color, bold, anchor: 'middle' });
  }

  box({ x, y, w, h, fill, stroke, title, sub, titleColor = C.ink, subColor = C.muted, titleSize = 18, subSize = 11, rx = 16, strokeWidth = 1.5 }) {
    this.addRect({ x, y, w, h, fill, stroke, strokeWidth, rx });
    const cx = x + w / 2;
    if (title && sub) {
      this.addTextOnly({ cx, cy: y + h / 2 - 9, text: title, size: titleSize, color: titleColor, bold: true, anchor: 'middle' });
      this.addTextOnly({ cx, cy: y + h / 2 + 13, text: sub, size: subSize, color: subColor, anchor: 'middle' });
    } else if (title) {
      this.addTextOnly({ cx, cy: y + h / 2, text: title, size: titleSize, color: titleColor, bold: true, anchor: 'middle' });
    }
  }

  stepNumber({ cx, cy, n, color }) {
    const r = 13;
    const id = `${this.name}-n${this.index}`;
    this.elements.push({
      id,
      type: 'ellipse',
      x: cx - r, y: cy - r, width: r * 2, height: r * 2,
      angle: 0,
      strokeColor: color,
      backgroundColor: color,
      fillStyle: 'solid',
      strokeWidth: 1,
      strokeStyle: 'solid',
      roughness: 0,
      opacity: 100,
      groupIds: [],
      frameId: null,
      index: this.nextIndex(),
      roundness: null,
      seed: this.nextSeed(),
      version: 1,
      versionNonce: this.nextSeed(),
      isDeleted: false,
      boundElements: null,
      updated: now,
      link: null,
      locked: false,
    });
    this.svg.push(`<circle cx="${cx}" cy="${cy}" r="${r}" fill="${color}"/>`);
    this.addTextOnly({ cx, cy: cy + 0.5, text: n, size: 12, color: C.white, bold: true, anchor: 'middle' });
  }

  finish(fileName) {
    this.svg.push(`</svg>`);
    const doc = {
      type: 'excalidraw',
      version: 2,
      source: 'https://excalidraw.com',
      elements: this.elements,
      appState: {
        viewBackgroundColor: '#ffffff',
        gridSize: 20,
      },
      files: {},
    };
    fs.writeFileSync(path.join(OUT, `${fileName}.excalidraw`), JSON.stringify(doc, null, 2));
    fs.writeFileSync(path.join(OUT, `${fileName}.svg`), this.svg.join('\n'));
    console.log(`${fileName}: ${this.elements.length} elements`);
  }
}

// ##########################################################################
// A. System architecture
// ##########################################################################
{
  const d = new Diagram(
    'system-architecture', 1200, 560,
    'MyWaveQt 四层架构',
    '音频输入 → 解码 → 特征 → 判决 → 可视化；UI 向下调用，数据向上汇聚',
  );

  const rows = [
    {
      y: 120, name: 'UI 展示层', sub: 'QCustomPlot + OpenGL',
      fill: C.softBlue, accent: C.blue,
      chips: [
        ['多泳道视图', 220],
        ['参数面板', 170],
        ['图表导出', 150],
      ],
    },
    {
      y: 224, name: 'VAD 调度层', sub: 'VadEngine 外观',
      fill: C.softTeal, accent: C.teal,
      chips: [
        ['VadEngine', 140],
        ['双门限', 105],
        ['能量', 85],
        ['ZCR', 80],
        ['倒谱', 90],
        ['Silero', 95],
      ],
    },
    {
      y: 328, name: 'DSP 计算层', sub: '纯数学核心',
      fill: C.softAmber, accent: C.amber,
      chips: [
        ['FramedSignal', 150],
        ['FFTW PlanCache', 160],
        ['特征算子', 140],
        ['parallelFor', 130],
      ],
    },
    {
      y: 432, name: 'Core 服务层', sub: '解码 · 播放 · 缓存',
      fill: C.softGray, accent: C.muted,
      chips: [
        ['miniaudio 解码', 170],
        ['AudioPlayer', 140],
        ['AudioStorage', 140],
        ['ChartCache', 130],
      ],
    },
  ];

  for (const row of rows) {
    d.addRect({ x: 120, y: row.y, w: 860, h: 70, fill: row.fill, stroke: row.accent, strokeWidth: 1.1, rx: 16 });
    d.addRect({ x: 120, y: row.y, w: 6, h: 70, fill: row.accent, stroke: row.accent, strokeWidth: 0, rx: 3 });
    d.addTextOnly({ cx: 145, cy: row.y + 27, text: row.name, size: 18, color: C.ink, bold: true, anchor: 'start' });
    d.addTextOnly({ cx: 145, cy: row.y + 49, text: row.sub, size: 10.5, color: C.muted, anchor: 'start' });
    d.addLine({ x1: 300, y1: row.y + 12, x2: 300, y2: row.y + 58, color: C.line, width: 1 });

    let cx = 318;
    for (const [label, w] of row.chips) {
      d.chip({
        x: cx, y: row.y + 19, w, h: 32, label,
        fill: C.white, stroke: row.accent, color: C.ink, size: 12.5,
      });
      cx += w + 8;
    }
  }

  // Vertical data flow between layers
  const flow = [
    { y1: 432, y2: 398, label: 'PCM 帧' },
    { y1: 328, y2: 294, label: '特征曲线' },
    { y1: 224, y2: 190, label: '判决区间' },
  ];
  for (const f of flow) {
    d.addArrow({
      x1: 620, y1: f.y1, x2: 620, y2: f.y2,
      color: C.muted, width: 2, label: f.label,
      labelX: 675, labelY: (f.y1 + f.y2) / 2,
    });
  }

  // Input / output
  d.box({
    x: 1020, y: 126, w: 150, h: 66,
    fill: C.softTeal, stroke: C.teal,
    title: '输出', sub: '语音段 / 曲线 / 图谱',
    titleColor: C.teal, titleSize: 16, subSize: 9.5,
  });
  d.box({
    x: 1020, y: 434, w: 150, h: 66,
    fill: C.softGray, stroke: C.muted,
    title: '输入', sub: 'WAV / MP3 / FLAC / OGG',
    titleColor: C.navy, titleSize: 16, subSize: 9.2,
  });
  d.addArrow({ x1: 980, y1: 159, x2: 1018, y2: 159, color: C.teal, width: 2, label: '渲染', labelX: 999, labelY: 145 });
  d.addArrow({ x1: 1018, y1: 467, x2: 982, y2: 467, color: C.muted, width: 2, label: '解码', labelX: 1000, labelY: 453 });

  d.addTextOnly({
    cx: 600, cy: 545,
    text: '同一视口内完成特征、门限与判决的三轨对照；参数改动即时重算。',
    size: 11.5, color: C.muted, anchor: 'middle',
  });

  d.finish('system-architecture');
}

// ##########################################################################
// B. VAD pipeline
// ##########################################################################
{
  const d = new Diagram(
    'vad-pipeline', 1200, 480,
    'VAD 处理流水线',
    '从音频到语音段：特征 → 相对门限 → 持续判决 → 扩展与合并',
  );

  const Y = 180;
  const H = 86;
  const steps = [
    { x: 40, w: 140, fill: C.softGray, stroke: C.muted, title: '音频输入', sub: '多格式解码', color: C.ink, accent: C.muted },
    { x: 210, w: 170, fill: C.softBlue, stroke: C.blue, title: '分帧加窗', sub: '512 点 · 50% 帧移', color: C.ink, accent: C.blue },
    { x: 410, w: 190, fill: C.softTeal, stroke: C.teal, title: '能量 + ZCR', sub: '短时能量 / 过零率', color: C.ink, accent: C.teal },
    { x: 630, w: 190, fill: C.softAmber, stroke: C.amber, title: '峰值相对门限', sub: 'Eh / El / Zth', color: C.ink, accent: C.amber },
    { x: 850, w: 170, fill: C.navy, stroke: C.navy, title: '持续 + 扩展', sub: 'Nv=3 帧 · El + Zth', color: C.white, accent: C.navy },
  ];

  steps.forEach((s, i) => {
    d.box({
      x: s.x, y: Y, w: s.w, h: H,
      fill: s.fill, stroke: s.stroke,
      title: s.title, sub: s.sub,
      titleColor: s.color, subColor: i === 4 ? '#CBD5E1' : C.muted,
      titleSize: 17, subSize: 10.5,
    });
    d.stepNumber({ cx: s.x + 18, cy: Y - 18, n: `0${i + 1}`, color: s.accent });
  });

  // Main arrows
  for (let i = 0; i < steps.length - 1; i++) {
    const a = steps[i];
    const b = steps[i + 1];
    d.addArrow({ x1: a.x + a.w + 4, y1: Y + H / 2, x2: b.x - 6, y2: Y + H / 2, color: C.muted, width: 2 });
  }

  // Output
  d.box({
    x: 1050, y: Y, w: 120, h: H,
    fill: C.softTeal, stroke: C.teal,
    title: '语音段', sub: '起点 / 终点',
    titleColor: C.teal, titleSize: 16, subSize: 10,
  });
  d.addArrow({ x1: 1024, y1: Y + H / 2, x2: 1046, y2: Y + H / 2, color: C.teal, width: 2 });

  // Callouts
  d.addRect({ x: 590, y: 330, w: 240, h: 68, fill: C.white, stroke: C.amber, strokeWidth: 1.2, rx: 12 });
  d.addTextOnly({ cx: 710, cy: 351, text: '参数可调', size: 13, color: C.amber, bold: true, anchor: 'middle' });
  d.addTextOnly({ cx: 710, cy: 373, text: '低/高能量偏移 · ZCR 比例 · 最小帧数', size: 9.2, color: C.muted, anchor: 'middle' });
  d.addArrow({ x1: 710, y1: 328, x2: 725, y2: 270, color: C.amber, width: 1.2, dashed: true });

  d.addRect({ x: 850, y: 330, w: 270, h: 68, fill: C.white, stroke: C.muted, strokeWidth: 1.2, rx: 12 });
  d.addTextOnly({ cx: 985, cy: 351, text: '复核策略', size: 13, color: C.navy, bold: true, anchor: 'middle' });
  d.addTextOnly({ cx: 985, cy: 373, text: '非平稳噪声 → 基音周期性 / Silero', size: 9.2, color: C.muted, anchor: 'middle' });
  d.addArrow({ x1: 985, y1: 328, x2: 935, y2: 270, color: C.muted, width: 1.2, dashed: true });

  d.addTextOnly({
    cx: 600, cy: 445,
    text: '能量、ZCR 与判决三轨同屏对齐；参数改动后即时重算。',
    size: 11.5, color: C.muted, anchor: 'middle',
  });

  d.finish('vad-pipeline');
}

console.log(`diagrams written to ${OUT}`);
