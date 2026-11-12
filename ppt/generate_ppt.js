const pptxgen = require('pptxgenjs');
const path = require('path');

const pres = new pptxgen();
pres.layout = 'LAYOUT_16x9';
pres.author = '范恩波';
pres.company = '南京理工大学 · 电子工程与光电技术学院';
pres.subject = '语音信号时间域特征对比分析及端点检测 · 结题答辩';
pres.title = '语音信号时间域特征对比分析及端点检测';
pres.lang = 'zh-CN';
pres.theme = {
  headFontFace: 'Microsoft YaHei',
  bodyFontFace: 'Microsoft YaHei',
  lang: 'zh-CN',
};

// ── Design tokens ─────────────────────────────────────────────────────────
const C = {
  ink:       '0B1220',
  navy:      '14315C',
  blue:      '2F5FE3',
  teal:      '0E7C75',
  amber:     'B45309',
  rose:      'B4233C',
  muted:     '5B6B82',
  softMuted: '94A3B8',
  line:      'DCE3EC',
  paper:     'F8FAFC',
  white:     'FFFFFF',
  softBlue:  'EEF3FB',
  softTeal:  'E8F4F2',
  softAmber: 'FBF0E4',
  softRose:  'FBE9EC',
  softGray:  'F1F5F9',
};

const F = {
  zh: 'Microsoft YaHei',
  en: 'Calibri',
  num: 'Arial',
};

const M = 0.68;
const CW = 8.64;
const TOTAL = 7;

// ── Helpers ───────────────────────────────────────────────────────────────
function tx(slide, text, x, y, w, h, opts = {}) {
  slide.addText(text, {
    x, y, w, h,
    fontFace: F.zh,
    fontSize: 10,
    color: C.ink,
    margin: 0,
    valign: 'top',
    ...opts,
  });
}

function rule(slide, x, y, w, color = C.line, width = 0.75) {
  slide.addShape(pres.ShapeType.line, {
    x, y, w, h: 0,
    line: { color, width },
  });
}

function vrule(slide, x, y, h, color = C.line, width = 0.75) {
  slide.addShape(pres.ShapeType.line, {
    x, y, w: 0, h,
    line: { color, width },
  });
}

function card(slide, x, y, w, h, opts = {}) {
  slide.addShape(pres.ShapeType.roundRect, {
    x, y, w, h,
    fill: { color: opts.fill || C.white },
    line: { color: opts.border || C.line, width: opts.borderWidth || 0.75 },
    rectRadius: opts.radius !== undefined ? opts.radius : 0.06,
  });
}

function circle(slide, x, y, d, fill, label, fontSize = 8.5) {
  slide.addShape(pres.ShapeType.ellipse, {
    x, y, w: d, h: d,
    fill: { color: fill },
    line: { color: fill, width: 0 },
  });
  tx(slide, label, x, y + 0.02, d, d - 0.02, {
    fontSize,
    color: C.white,
    bold: true,
    align: 'center',
    valign: 'middle',
  });
}

function header(slide, section, title, page) {
  tx(slide, section, M, 0.30, CW, 0.18, {
    fontSize: 8,
    color: C.blue,
    bold: true,
    charSpacing: 1.1,
  });
  tx(slide, title, M, 0.50, CW, 0.48, {
    fontSize: 22,
    color: C.navy,
    bold: true,
    fit: 'shrink',
  });
  rule(slide, M, 1.12, CW);
  tx(slide, 'MyWaveQt · 语音信号时间域特征对比分析及端点检测', M, 5.33, 6.1, 0.16, {
    fontSize: 7,
    color: C.muted,
  });
  tx(slide, `${String(page).padStart(2, '0')} / ${String(TOTAL).padStart(2, '0')}`, 8.9, 5.33, 0.42, 0.16, {
    fontSize: 7,
    color: C.muted,
    align: 'right',
  });
}

function appendixHeader(slide, section, title, pageLabel) {
  tx(slide, section, M, 0.30, CW, 0.18, {
    fontSize: 8,
    color: C.teal,
    bold: true,
    charSpacing: 1.1,
  });
  tx(slide, title, M, 0.50, CW, 0.48, {
    fontSize: 22,
    color: C.navy,
    bold: true,
    fit: 'shrink',
  });
  rule(slide, M, 1.12, CW);
  tx(slide, 'MyWaveQt · 附录 · 答辩备用', M, 5.33, 6.1, 0.16, {
    fontSize: 7,
    color: C.muted,
  });
  tx(slide, pageLabel, 8.55, 5.33, 0.75, 0.16, {
    fontSize: 7,
    color: C.muted,
    align: 'right',
  });
}

function stat(slide, x, y, w, number, label, color) {
  tx(slide, number, x, y, w, 0.40, {
    fontFace: F.num,
    fontSize: 21,
    color,
    bold: true,
  });
  tx(slide, label, x, y + 0.40, w, 0.18, {
    fontSize: 7.5,
    color: C.muted,
  });
}

// ##########################################################################
// 01 · Cover
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.ink };

  // Subtle editorial grid
  for (const x of [2.45, 4.9, 7.35]) {
    vrule(slide, x, 0, 5.625, '1B2740', 0.5);
  }
  slide.addShape(pres.ShapeType.ellipse, {
    x: 6.55, y: -0.85, w: 3.5, h: 3.5,
    fill: { color: C.teal, transparency: 92 },
    line: { color: C.teal, transparency: 100, width: 0 },
  });

  tx(slide, '大学生创新创业训练计划 · 结题答辩', 0.8, 0.70, 5.4, 0.22, {
    fontSize: 9.2,
    color: '5EEAD4',
    bold: true,
    charSpacing: 1.4,
  });

  tx(slide, '语音信号时间域特征\n对比分析与端点检测', 0.8, 0.98, 5.35, 1.30, {
    fontSize: 30,
    color: C.white,
    bold: true,
    lineSpacingMultiple: 1.12,
  });

  rule(slide, 0.8, 2.40, 1.0, '5EEAD4', 2);

  tx(slide, 'Time-Domain Feature Analysis and Voice Activity Detection for Speech Signals',
     0.8, 2.56, 5.15, 0.42, {
    fontFace: F.en,
    fontSize: 8.8,
    color: C.softMuted,
    italic: true,
    lineSpacingMultiple: 1.1,
  });

  tx(slide, '从时域机理  →  相对门限判决  →  可交互分析系统',
     0.8, 3.08, 5.2, 0.28, {
    fontSize: 10.2,
    color: 'CBD5E1',
  });

  // Meta row: no boxes, thin separators
  const metaY = 4.26;
  const metas = [
    { label: '项目负责人', val: '范恩波' },
    { label: '指导教师', val: '张二华 老师' },
    { label: '团队成员', val: '刘金戈 · 彭欣雨' },
    { label: '项目编号', val: '2025102880102' },
  ];
  const colW = 1.30;
  metas.forEach((m, i) => {
    const x = 0.8 + i * (colW + 0.06);
    tx(slide, m.label, x, metaY, colW, 0.16, {
      fontSize: 7.2, color: '64748B',
    });
    tx(slide, m.val, x, metaY + 0.20, colW, 0.24, {
      fontSize: 9.0, color: C.white, bold: true, fit: 'shrink',
    });
    if (i < metas.length - 1) {
      vrule(slide, x + colW + 0.01, metaY + 0.02, 0.38, '334155', 0.6);
    }
  });

  tx(slide, '2026 年 11 月 · 南京理工大学', 0.8, 5.08, 4.0, 0.18, {
    fontSize: 7.4, color: '64748B',
  });

  tx(slide, 'VAD', 6.12, 4.30, 3.18, 0.80, {
    fontFace: F.en, fontSize: 64, color: '111C30', bold: true,
    align: 'right', charSpacing: 1.5,
  });

  // Signal preview
  tx(slide, 'SIGNAL / VAD', 6.34, 1.06, 3.0, 0.18, {
    fontSize: 7.2, color: '5EEAD4', bold: true, charSpacing: 1.6,
  });
  slide.addImage({
    path: path.join(__dirname, 'assets', 'cover_signal.png'),
    x: 6.30, y: 1.30, w: 3.18, h: 1.87,
  });
  tx(slide, '峰值相对门限 · 语音段着色', 6.34, 3.28, 3.1, 0.18, {
    fontSize: 7.4, color: '64748B',
  });
  vrule(slide, 6.14, 1.08, 2.34, '2DD4BF', 1.2);

  slide.addNotes(
    '【0:00 - 0:30】\n' +
    '尊敬的各位评委老师好，我是项目负责人范恩波。今天代表团队汇报大学生创新训练项目《语音信号时间域特征对比分析及端点检测》。' +
    '项目由张二华老师指导，成员为刘金戈、彭欣雨。' +
    '我们围绕任务书中的三件事展开：提取短时能量与过零率、对比无声/清音/浊音的差异、用双门限检测有声段起止，并把算法做成可交互的分析系统 MyWaveQt。'
  );
}

// ##########################################################################
// 02 · Task → questions → closure
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  header(slide, '01 / 背景与任务', '从任务书到交付：三个问题，一条闭环', 2);

  // Left: task requirements
  tx(slide, '任务书要求', M, 1.34, 3.0, 0.18, {
    fontSize: 8.5, color: C.blue, bold: true, charSpacing: 1,
  });

  const reqs = [
    { n: '01', t: '特征提取', d: '短时能量、短时过零率、基音周期' },
    { n: '02', t: '三态对比', d: '无声 / 清音 / 浊音的物理指征差异' },
    { n: '03', t: '端点检测', d: '双门限定位有声段起止，并开发软件' },
  ];
  reqs.forEach((r, i) => {
    const y = 1.62 + i * 0.83;
    tx(slide, r.n, M, y, 0.32, 0.18, {
      fontFace: F.num, fontSize: 8.5, color: C.blue, bold: true,
    });
    tx(slide, r.t, M + 0.38, y - 0.02, 2.55, 0.22, {
      fontSize: 10.2, color: C.ink, bold: true,
    });
    tx(slide, r.d, M + 0.38, y + 0.23, 2.62, 0.38, {
      fontSize: 8.2, color: C.muted, lineSpacingMultiple: 1.12,
    });
    if (i < reqs.length - 1) rule(slide, M + 0.38, y + 0.70, 2.56);
  });

  card(slide, M, 4.02, 3.16, 0.40, { fill: C.softTeal, border: 'C8E6E2', radius: 0.05 });
  tx(slide, '✓  3 项要求全部落地  +  1 套自研软件', M + 0.14, 4.10, 2.95, 0.22, {
    fontSize: 8.3, color: C.teal, bold: true, fit: 'shrink',
  });

  // Right: three questions
  tx(slide, '三个必须回答的问题', 4.14, 1.34, 4.9, 0.18, {
    fontSize: 8.5, color: C.amber, bold: true, charSpacing: 1,
  });

  const questions = [
    {
      n: '01',
      t: '低信噪比下，固定门限为何失效？',
      d: '绝对幅值与经验阈值无法跟随录音电平与噪声变化，弱语音会被淹没。',
    },
    {
      n: '02',
      t: '清辅音和闭塞音为何被切掉？',
      d: '清音能量弱但 ZCR 高；字词间停顿短，单一能量判决会把词句撕裂。',
    },
    {
      n: '03',
      t: '如何让算法结论可验证？',
      d: '特征、门限与判决需同屏对齐，参数可调、结果可复现。',
    },
  ];
  questions.forEach((q, i) => {
    const y = 1.66 + i * 0.87;
    tx(slide, q.n, 4.14, y - 0.02, 0.46, 0.30, {
      fontFace: F.num, fontSize: 18, color: C.amber, bold: true,
    });
    tx(slide, q.t, 4.64, y - 0.03, 4.45, 0.24, {
      fontSize: 11.2, color: C.ink, bold: true, fit: 'shrink',
    });
    tx(slide, q.d, 4.64, y + 0.25, 4.42, 0.42, {
      fontSize: 8.5, color: C.muted, lineSpacingMultiple: 1.12,
    });
    if (i < questions.length - 1) rule(slide, 4.64, y + 0.74, 4.40);
  });

  // Bottom conclusion band
  slide.addShape(pres.ShapeType.rect, {
    x: M, y: 4.54, w: CW, h: 0.50,
    fill: { color: C.navy }, line: { color: C.navy, width: 0 },
  });
  slide.addShape(pres.ShapeType.rect, {
    x: M, y: 4.54, w: 0.06, h: 0.50,
    fill: { color: '2DD4BF' }, line: { color: '2DD4BF', width: 0 },
  });
  tx(slide, '本项目的回答：把物理机理、判决逻辑与可视化证据，放在同一张工作台上。',
     M + 0.22, 4.66, CW - 0.35, 0.24, {
    fontSize: 10.2, color: C.white, bold: true, fit: 'shrink',
  });

  slide.addNotes(
    '【0:30 - 1:10】\n' +
    '任务书要求很明确：提取短时能量和过零率，分析无声、清音、浊音三类状态的差异，用双门限找到有声段起止，并开发配套软件。' +
    '真正做起来，我们把它归结为三个问题：第一，固定门限为什么在低信噪比下失效；第二，清辅音和字词间闭塞音为什么容易被切掉；第三，算法结论怎样才能被验证。' +
    '我们的回答是：不把算法藏在脚本里，而是把特征、门限和判决放到同一个可视工作台上。'
  );
}

// ##########################################################################
// 03 · Time-domain features
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  header(slide, '02 / 机理', '时域特征：三种声学状态的物理指征', 3);

  card(slide, 0.58, 1.26, 5.78, 3.10, { fill: C.white });
  slide.addImage({
    path: path.join(__dirname, 'assets', 'feature_states.png'),
    x: 0.70, y: 1.33, w: 5.54, h: 2.96,
  });
  tx(slide, '合成语音示意 · 16 kHz · 帧长 512 / 帧移 256 · 阴影为检测到的语音段',
     0.72, 4.42, 5.55, 0.18, {
    fontSize: 7.2, color: C.muted,
  });

  // Right: three states
  tx(slide, '三态判据', 6.55, 1.34, 2.75, 0.18, {
    fontSize: 8.5, color: C.navy, bold: true, charSpacing: 1,
  });

  const states = [
    { name: '无声', color: C.muted, soft: C.softGray, d: '能量极低；ZCR 随机；无基音周期性' },
    { name: '清音', color: C.amber, soft: C.softAmber, d: '能量较弱；ZCR 很高；无周期性振动' },
    { name: '浊音', color: C.teal, soft: C.softTeal, d: '能量最强；ZCR 较低；基音周期明显' },
  ];
  states.forEach((s, i) => {
    const y = 1.66 + i * 0.76;
    slide.addShape(pres.ShapeType.rect, {
      x: 6.55, y, w: 0.05, h: 0.62,
      fill: { color: s.color }, line: { color: s.color, width: 0 },
    });
    tx(slide, s.name, 6.70, y - 0.02, 0.55, 0.22, {
      fontSize: 10.2, color: s.color, bold: true,
    });
    tx(slide, s.d, 6.70, y + 0.24, 2.55, 0.38, {
      fontSize: 8.2, color: C.muted, lineSpacingMultiple: 1.12,
    });
  });

  card(slide, 6.55, 4.02, 2.75, 0.62, { fill: C.softBlue, border: 'D5E0F5', radius: 0.05 });
  tx(slide, '能量定主体，ZCR 找清音；\n基音周期性补足浊音证据。', 6.68, 4.10, 2.50, 0.46, {
    fontSize: 8.4, color: C.blue, bold: true, lineSpacingMultiple: 1.15,
  });

  slide.addNotes(
    '【1:10 - 1:50】\n' +
    '这一页把三种状态放在同一时间轴上。最上面是波形，中间是短时能量，下面是过零率。' +
    '无声段能量低、过零率随机；清音段能量弱，但过零率明显升高；浊音段能量最强，过零率反而较低，并且存在基音周期性。' +
    '所以我们的判据不是单看能量，而是：能量定主体，ZCR 找清音，基音周期性补足浊音证据。'
  );
}

// ##########################################################################
// 04 · Decision logic
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  header(slide, '03 / 算法', '判决逻辑：峰值相对门限的四步状态机', 4);

  // Flow spine
  vrule(slide, 0.92, 1.48, 3.08, C.line, 1.0);

  const steps = [
    {
      n: '1', color: C.blue,
      t: '分帧与特征计算',
      d: '帧长 512、帧移 50%；逐帧算能量与 ZCR。',
    },
    {
      n: '2', color: C.teal,
      t: '峰值相对门限',
      d: 'Eh=Emax−10dB；El=Emax−20dB；Zth=0.2ZCRmax。',
    },
    {
      n: '3', color: C.amber,
      t: '高门限持续判决',
      d: '连续 Nv = 3 帧高于 Eh，才确立语音核心，滤除孤立脉冲。',
    },
    {
      n: '4', color: C.navy,
      t: '双向扩展与合并',
      d: '低门限向两侧扩展，ZCR 再搜寻清辅音；重叠或相邻段合并。',
    },
  ];
  steps.forEach((s, i) => {
    const y = 1.36 + i * 0.78;
    circle(slide, 0.78, y + 0.01, 0.29, s.color, s.n, 8.2);
    tx(slide, s.t, 1.20, y - 0.02, 3.20, 0.22, {
      fontSize: 10.2, color: C.ink, bold: true,
    });
    tx(slide, s.d, 1.20, y + 0.24, 3.25, 0.40, {
      fontSize: 8.3, color: C.muted, lineSpacingMultiple: 1.10,
    });
  });

  // Figure
  card(slide, 4.72, 1.26, 4.58, 3.10, { fill: C.white });
  slide.addImage({
    path: path.join(__dirname, 'assets', 'vad_threshold.png'),
    x: 4.82, y: 1.32, w: 4.38, h: 2.78,
  });
  tx(slide, '同一帧长下的能量 / ZCR 判决示意；阴影为输出语音段',
     4.84, 4.18, 4.30, 0.18, {
    fontSize: 7.2, color: C.muted,
  });

  // Honest scope note
  slide.addShape(pres.ShapeType.rect, {
    x: M, y: 4.66, w: CW, h: 0.46,
    fill: { color: C.softAmber }, line: { color: 'F0D9BE', width: 0.6 },
  });
  slide.addShape(pres.ShapeType.rect, {
    x: M, y: 4.66, w: 0.05, h: 0.46,
    fill: { color: C.amber }, line: { color: C.amber, width: 0 },
  });
  tx(slide, '门限随录音峰值归一化，避免绝对幅值依赖；参数可在软件中实时调整。\n面对非平稳噪声，仍需结合基音周期性或深度模型复核。',
     M + 0.18, 4.72, CW - 0.30, 0.36, {
    fontSize: 8.0, color: C.amber, lineSpacingMultiple: 1.08, fit: 'shrink',
  });

  slide.addNotes(
    '【1:50 - 2:35】\n' +
    '判决逻辑可以概括为四步。第一步，按 512 点帧长、50% 帧移分帧，逐帧算能量和过零率。' +
    '第二步，不做绝对阈值，而是以整段音频的峰值能量为参照：高门限取峰值下 10 dB，低门限取 20 dB，过零率门限取最大值的 20%。' +
    '第三步，必须连续 3 帧高于高门限，才认为进入语音核心，这样可以滤掉敲击一类的孤立脉冲。' +
    '第四步，先用低门限向两侧扩展浊音边缘，再用过零率向边界外搜寻清辅音，最后合并重叠或相邻的语音段。' +
    '需要说明的是，这种相对门限解决的是录音电平差异，不是万能的噪声自适应；非平稳噪声下还要结合基音或深度模型复核。'
  );
}

// ##########################################################################
// 05 · System
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  header(slide, '04 / 系统', '系统实现：从算法到可交互工作台', 5);

  tx(slide, '四层解耦', M, 1.34, 3.0, 0.18, {
    fontSize: 8.5, color: C.blue, bold: true, charSpacing: 1,
  });

  const layers = [
    {
      name: 'UI 展示层', soft: C.softBlue, accent: C.blue,
      d: 'QCustomPlot + OpenGL · 多泳道层叠 · 深浅主题',
    },
    {
      name: 'VAD 调度层', soft: C.softTeal, accent: C.teal,
      d: 'VadEngine 外观 · 5 种检测器 · 15 项可调参数',
    },
    {
      name: 'DSP 计算层', soft: C.softAmber, accent: C.amber,
      d: 'FramedSignal 连续帧 · FFTW PlanCache · parallelFor',
    },
    {
      name: 'Core 服务层', soft: C.softGray, accent: C.muted,
      d: 'miniaudio 解码 · 低延迟播放 · ChartCache',
    },
  ];
  layers.forEach((l, i) => {
    const y = 1.64 + i * 0.78;
    card(slide, M, y, 4.52, 0.66, { fill: l.soft, border: 'E3E9F1', radius: 0.05 });
    slide.addShape(pres.ShapeType.rect, {
      x: M, y, w: 0.05, h: 0.66,
      fill: { color: l.accent }, line: { color: l.accent, width: 0 },
    });
    tx(slide, l.name, M + 0.16, y + 0.09, 0.95, 0.22, {
      fontSize: 9.2, color: C.ink, bold: true,
    });
    tx(slide, l.d, M + 1.20, y + 0.10, 3.16, 0.46, {
      fontSize: 7.5, color: C.muted, lineSpacingMultiple: 1.10,
    });
  });

  // UI wireframe
  card(slide, 5.48, 1.30, 3.82, 3.36, { fill: C.white });
  tx(slide, '界面结构', 5.60, 1.38, 2.0, 0.16, {
    fontSize: 7.4, color: C.muted, bold: true, charSpacing: 0.8,
  });

  slide.addShape(pres.ShapeType.rect, {
    x: 5.58, y: 1.58, w: 3.62, h: 0.25,
    fill: { color: C.softBlue }, line: { color: 'D5E0F5', width: 0.5 },
  });
  tx(slide, '打开   文件选择   + 分析曲线', 5.66, 1.62, 3.45, 0.16, {
    fontSize: 6.8, color: C.blue, bold: true,
  });

  slide.addShape(pres.ShapeType.rect, {
    x: 5.58, y: 1.90, w: 3.62, h: 1.62,
    fill: { color: 'FBFCFE' }, line: { color: C.line, width: 0.5 },
  });
  for (const [label, y] of [['波形', 1.98], ['能量', 2.42], ['ZCR', 2.86]]) {
    tx(slide, label, 5.64, y, 0.34, 0.14, {
      fontSize: 6.0, color: C.muted,
    });
    rule(slide, 6.02, y + 0.16, 3.08, 'E8EDF4', 0.5);
  }
  slide.addImage({
    path: path.join(__dirname, 'assets', 'ui_lanes.png'),
    x: 6.04, y: 1.96, w: 3.04, h: 1.50,
  });
  // playhead tick
  vrule(slide, 7.12, 1.96, 1.50, C.rose, 0.8);

  // Bottom parameter panel
  slide.addShape(pres.ShapeType.rect, {
    x: 5.58, y: 3.60, w: 3.62, h: 0.94,
    fill: { color: C.softGray }, line: { color: 'E3E9F1', width: 0.5 },
  });
  tx(slide, 'VAD 参数', 5.66, 3.68, 1.40, 0.16, {
    fontSize: 6.8, color: C.navy, bold: true,
  });
  rule(slide, 5.68, 3.84, 1.36, 'C8D2E0', 0.5);
  const params = [
    ['高/低能量偏移', '−20 / −10 dB'],
    ['ZCR 比例', '0.20'],
    ['最小语音帧数', '3'],
  ];
  params.forEach((p, i) => {
    const y = 3.88 + i * 0.19;
    tx(slide, p[0], 5.68, y, 0.82, 0.14, {
      fontSize: 5.4, color: C.muted,
    });
    tx(slide, p[1], 6.50, y, 0.60, 0.14, {
      fontSize: 5.4, color: C.navy, bold: true, align: 'right',
    });
  });

  slide.addShape(pres.ShapeType.roundRect, {
    x: 7.18, y: 3.72, w: 1.92, h: 0.28,
    fill: { color: C.blue }, line: { color: C.blue, width: 0 },
    rectRadius: 0.03,
  });
  tx(slide, '运行 / 算法对比', 7.22, 3.78, 1.84, 0.16, {
    fontSize: 6.6, color: C.white, bold: true, align: 'center',
  });
  tx(slide, '参考双门限  ·  纯能量  ·  纯 ZCR\n倒谱  ·  Silero VAD', 7.20, 4.10, 1.86, 0.32, {
    fontSize: 5.8, color: C.muted, lineSpacingMultiple: 1.0,
  });

  tx(slide, '界面结构示意：工具栏 · 多泳道 · 参数面板', 5.50, 4.72, 3.78, 0.16, {
    fontSize: 7.0, color: C.muted,
  });

  tx(slide, '4 层架构   ·   5 种 VAD   ·   23 类分析曲线   ·   2 个测试目标',
     M, 4.92, 7.9, 0.20, {
    fontSize: 8.2, color: C.muted, bold: true, charSpacing: 0.4,
  });

  slide.addNotes(
    '【2:35 - 3:15】\n' +
    '系统按四层解耦：UI 层用 QCustomPlot 和 OpenGL 做多泳道显示；VAD 层用 VadEngine 统一调度五种检测器；DSP 层负责分帧、FFT、倒谱和特征计算；Core 层负责解码、播放和缓存。' +
    '工程上做了三件事：用 FramedSignal 把帧数据放在一块连续内存里；用 FFTW PlanCache 复用 FFT 规划；用 QtConcurrent 把重计算放到后台，并用极值降采样控制视口数据量。' +
    '右侧是界面结构示意：上方工具栏、中间多泳道、下方参数面板。实际截图可以在答辩前替换这一块。'
  );
}

// ##########################################################################
// 06 · Validation matrix
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  header(slide, '05 / 验证', '验证：五种检测器，同一套证据链', 6);

  const stats = [
    { n: '5', l: '种 VAD 方案', c: C.blue },
    { n: '23', l: '类分析曲线', c: C.teal },
    { n: '15', l: '项 VAD 可调参数', c: C.amber },
    { n: '3', l: '轨判决视图', c: C.navy },
  ];
  stats.forEach((s, i) => {
    const x = M + i * 2.18;
    stat(slide, x, 1.34, 2.0, s.n, s.l, s.c);
    if (i < stats.length - 1) vrule(slide, x + 2.08, 1.38, 0.58);
  });

  const headerRow = ['方案', '核心判据', '可调参数', '优势', '局限'];
  const rows = [
    ['双门限（参考）', '能量 + ZCR；峰值相对门限', '低/高能量偏移、ZCR 比例、最小帧数', '物理意义清晰、计算轻量', '依赖峰值，噪声下需调参'],
    ['纯能量', '帧能量阈值 + hangover', '能量阈值、挂起帧数', '实现最简单、延迟低', '清音与弱语音易漏检'],
    ['纯 ZCR', 'ZCR 阈值 + 能量地板', 'ZCR 阈值、能量地板', '对清音与高频敏感', '宽带噪声下虚警偏高'],
    ['倒谱', '倒谱峰比 + 能量门限', '峰比、能量门限、最小帧数', '利用周期性，抗噪性较好', '计算量高于时域特征'],
    ['Silero', '神经网络帧级概率', '阈值、最短语音/静音', '泛化强，作为深度基准', '依赖模型与推理运行时'],
  ];

  const tableRows = [
    headerRow.map((h) => ({
      text: h,
      options: {
        fill: { color: C.navy },
        color: C.white,
        bold: true,
        fontFace: F.zh,
        fontSize: 8.2,
        align: 'center',
        valign: 'middle',
        border: { pt: 0.5, color: C.navy },
      },
    })),
    ...rows.map((row, r) => row.map((cell, c) => ({
      text: cell,
      options: {
        fill: { color: r % 2 === 0 ? C.white : C.softGray },
        color: c === 0 ? C.navy : C.muted,
        bold: c === 0,
        fontFace: F.zh,
        fontSize: 7.9,
        align: 'left',
        valign: 'middle',
        margin: 0.045,
        border: { pt: 0.5, color: C.line },
      },
    }))),
  ];

  slide.addTable(tableRows, {
    x: M, y: 2.18, w: CW,
    colW: [1.35, 2.00, 1.75, 1.65, 1.89],
    rowH: 0.37,
    autoPage: false,
  });

  // Workflow strip
  slide.addShape(pres.ShapeType.rect, {
    x: M, y: 4.62, w: CW, h: 0.34,
    fill: { color: C.softBlue }, line: { color: 'D5E0F5', width: 0.5 },
  });
  tx(slide, '加载音频  →  选择算法  →  调整参数  →  运行  →  三轨对照',
     M + 0.16, 4.69, CW - 0.30, 0.20, {
    fontSize: 8.4, color: C.blue, bold: true, align: 'center', fit: 'shrink',
  });

  tx(slide, '评测原则：控制变量、同屏对照、参数可复现；定量指标需在带标注测试集上统计。',
     M, 5.05, CW, 0.18, {
    fontSize: 7.4, color: C.muted,
  });

  slide.addNotes(
    '【3:15 - 3:55】\n' +
    '验证部分不堆砌数字，而是先说明证据链。软件内置五种检测器：参考双门限、纯能量、纯 ZCR、倒谱和 Silero。' +
    '它们共享同一套分帧、同一套参数面板和同一套三轨视图，所以可以在同一音频、同一帧长下直接对照。' +
    '这张矩阵比较的是方法边界，不是精度排名：双门限物理意义清晰、计算轻量，但依赖峰值；纯能量最简单，但清音容易漏；纯 ZCR 对清音敏感，但宽带噪声下虚警高；倒谱利用周期性，计算量更大；Silero 作为深度基准，泛化强但依赖模型运行时。' +
    '定量指标需要在带标注测试集上统计，这是下一步要补齐的工作。'
  );
}

// ##########################################################################
// 07 · Closing
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.ink };

  tx(slide, '06 / 总结', 0.8, 0.34, 3.0, 0.18, {
    fontSize: 8, color: '5EEAD4', bold: true, charSpacing: 1.2,
  });
  tx(slide, '结题：交付、分工与下一步', 0.8, 0.54, 8.4, 0.46, {
    fontSize: 22, color: C.white, bold: true, fit: 'shrink',
  });
  rule(slide, 0.8, 1.14, 8.4, '334155', 0.8);

  // Left: deliverables
  tx(slide, '已交付', 0.8, 1.36, 2.0, 0.18, {
    fontSize: 8.5, color: '5EEAD4', bold: true, charSpacing: 1,
  });

  const deliverables = [
    {
      t: 'MyWaveQt 跨平台分析系统',
      d: 'C++23 / Qt 6 · 源码、工程与模型资产完整交付',
    },
    {
      t: '时域特征与 VAD 对比工作台',
      d: '5 种检测器 · 23 类曲线 · 参数实时可调',
    },
    {
      t: '结题报告与测试资产',
      d: '算法说明、测试目标、项目文档归档',
    },
  ];
  deliverables.forEach((d, i) => {
    const y = 1.66 + i * 0.49;
    tx(slide, '✓', 0.80, y, 0.24, 0.20, {
      fontSize: 9, color: '2DD4BF', bold: true,
    });
    tx(slide, d.t, 1.06, y - 0.01, 4.75, 0.20, {
      fontSize: 9.6, color: C.white, bold: true,
    });
    tx(slide, d.d, 1.06, y + 0.19, 4.75, 0.18, {
      fontSize: 7.8, color: '94A3B8',
    });
  });

  rule(slide, 0.8, 3.18, 4.95, '334155', 0.6);

  tx(slide, '经费', 0.8, 3.30, 0.45, 0.18, {
    fontSize: 8.2, color: '5EEAD4', bold: true,
  });
  tx(slide, '600 元  ·  存储器件与资料复印  ·  执行率 100%', 1.28, 3.29, 4.55, 0.20, {
    fontSize: 8.6, color: C.white, bold: true, fit: 'shrink',
  });

  tx(slide, '分工', 0.8, 3.66, 0.45, 0.18, {
    fontSize: 8.2, color: '5EEAD4', bold: true,
  });
  const duties = [
    '范恩波 · 算法建模 / 双门限实现 / 系统后端',
    '刘金戈 · Qt 前端 / 交互与可视化 / 测试评估',
    '彭欣雨 · 文献与数据集 / 报告撰写 / 文档归档',
  ];
  duties.forEach((d, i) => {
    tx(slide, d, 1.28, 3.64 + i * 0.23, 4.55, 0.18, {
      fontSize: 7.9, color: '94A3B8',
    });
  });

  // Right: next steps
  tx(slide, '下一步', 6.22, 1.36, 2.0, 0.18, {
    fontSize: 8.5, color: 'FBBF24', bold: true, charSpacing: 1,
  });

  const next = [
    ['01', '标准化评测', '统计准确率、虚警率与漏检率'],
    ['02', '复杂噪声复核', '非平稳噪声、低信噪比与真实录音场景'],
    ['03', '轻量移植', '面向嵌入式端的定点化与实时性验证'],
  ];
  next.forEach((n, i) => {
    const y = 1.66 + i * 0.60;
    tx(slide, n[0], 6.22, y - 0.02, 0.30, 0.18, {
      fontFace: F.num, fontSize: 8.2, color: 'FBBF24', bold: true,
    });
    tx(slide, n[1], 6.56, y - 0.02, 2.55, 0.20, {
      fontSize: 9.2, color: C.white, bold: true,
    });
    tx(slide, n[2], 6.56, y + 0.19, 2.58, 0.34, {
      fontSize: 7.7, color: '94A3B8', lineSpacingMultiple: 1.08,
    });
  });

  // Thanks block
  card(slide, 6.22, 3.58, 2.88, 1.10, {
    fill: '0F172A', border: '334155', radius: 0.05,
  });
  tx(slide, '感谢张二华老师的悉心指导', 6.38, 3.72, 2.56, 0.20, {
    fontSize: 8.4, color: '5EEAD4', bold: true, align: 'center',
  });
  tx(slide, '请各位专家批评指正', 6.38, 4.02, 2.56, 0.30, {
    fontSize: 13.2, color: C.white, bold: true, align: 'center',
  });
  tx(slide, 'THANKS', 6.30, 4.78, 2.72, 0.38, {
    fontFace: F.en, fontSize: 24, color: '1B2740', bold: true,
    align: 'right', charSpacing: 1.5,
  });

  tx(slide, 'MyWaveQt · 语音信号时间域特征对比分析及端点检测', 0.8, 5.33, 6.1, 0.16, {
    fontSize: 7, color: '64748B',
  });
  tx(slide, '07 / 07', 8.9, 5.33, 0.42, 0.16, {
    fontSize: 7, color: '64748B', align: 'right',
  });

  slide.addNotes(
    '【3:55 - 4:30】\n' +
    '最后做结题总结。我们交付了三类成果：一套可运行的 MyWaveQt 系统、一套时域特征与五种 VAD 的对比工作台，以及结题报告、测试目标和项目文档。' +
    '经费 600 元全部用于存储器件与资料复印，执行率 100%。团队三人分别负责算法与后端、前端与测试、文献与文档。' +
    '下一步有两件事：一是补齐带标注测试集上的客观指标，二是把轻量算法移植到嵌入式端验证。' +
    '衷心感谢张二华老师的指导，感谢各位评委老师聆听，请各位老师批评指正。'
  );
}

// ##########################################################################
// A-01 · Appendix: system architecture
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  appendixHeader(slide, '附录 A / 系统架构', '四层解耦与双向数据流', 'A-01 / A-02');

  card(slide, 0.86, 1.18, 8.28, 3.91, { fill: C.white, border: C.line, borderWidth: 0.6, radius: 0.04 });
  slide.addImage({
    path: path.join(__dirname, 'diagrams', 'system-architecture.png'),
    x: 0.90, y: 1.22, w: 8.20, h: 3.83,
  });
  tx(slide, '可编辑源文件：ppt/diagrams/system-architecture.excalidraw · SVG / PNG 同目录',
     M, 5.12, CW, 0.18, {
    fontSize: 7.2, color: C.muted,
  });

  slide.addNotes(
    '【附录 · 按需展示】\n' +
    '如果评委追问系统如何分层、数据如何流动，可以切到这一页。' +
    '系统分为 UI、VAD、DSP、Core 四层：UI 层负责多泳道显示与参数交互；VAD 层用 VadEngine 统一调度五种检测器；DSP 层负责分帧、FFT、倒谱和特征计算；Core 层负责解码、播放和缓存。' +
    '调用方向是 UI 向下调用，数据方向是 Core 解码后的 PCM 帧向上汇聚为特征曲线和判决区间。'
  );
}

// ##########################################################################
// A-02 · Appendix: VAD pipeline
// ##########################################################################
{
  const slide = pres.addSlide();
  slide.background = { color: C.paper };
  appendixHeader(slide, '附录 B / VAD 细节', '从音频到语音段的五步流水线', 'A-02 / A-02');

  card(slide, 0.76, 1.26, 8.48, 3.44, { fill: C.white, border: C.line, borderWidth: 0.6, radius: 0.04 });
  slide.addImage({
    path: path.join(__dirname, 'diagrams', 'vad-pipeline.png'),
    x: 0.80, y: 1.30, w: 8.40, h: 3.36,
  });
  tx(slide, '图示：参考双门限的实际流程；参数可在软件中实时调整，非平稳噪声需结合基音或 Silero 复核。',
     M, 4.82, CW, 0.18, {
    fontSize: 7.2, color: C.muted,
  });

  slide.addNotes(
    '【附录 · 按需展示】\n' +
    '如果评委追问 VAD 的具体实现，可以切到这一页。' +
    '流程是：音频输入后按 512 点帧长、50% 帧移分帧，计算短时能量和过零率；以峰值为参照确定 Eh、El、Zth；连续 3 帧超过高门限才进入语音核心；再用低门限和过零率向两侧扩展；最后合并重叠或相邻段。' +
    '需要强调：这是峰值相对门限，不是基于噪声统计的自适应；参数可调，但非平稳噪声下仍需结合基音或 Silero 复核。'
  );
}

// ── Export ────────────────────────────────────────────────────────────────
const outFile = path.join(__dirname, '语音信号时间域特征对比分析及端点检测_结题答辩汇报.pptx');
pres.writeFile({ fileName: outFile })
  .then(() => console.log(`PPTX generated: ${outFile}`))
  .catch((err) => {
    console.error(err);
    process.exit(1);
  });
