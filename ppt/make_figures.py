import numpy as np
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib import font_manager

ASSETS = Path(__file__).resolve().parent / "assets"
ASSETS.mkdir(exist_ok=True)

# ── Theme ────────────────────────────────────────────────────────────────
INK      = "#0B1220"
NAVY     = "#14315C"
BLUE     = "#2F5FE3"
TEAL     = "#0E7C75"
AMBER    = "#B45309"
ROSE     = "#B4233C"
MUTED    = "#5B6B82"
LINE     = "#CBD5E1"
PAPER    = "#F8FAFC"

FONT = "Noto Sans CJK SC"
if FONT not in {f.name for f in font_manager.fontManager.ttflist}:
    FONT = "DejaVu Sans"
plt.rcParams.update({
    "font.family": FONT,
    "axes.unicode_minus": False,
    "figure.facecolor": "none",
    "axes.facecolor": "none",
    "savefig.facecolor": "none",
})

SR = 16000
FRAME = 512
HOP = 256
DUR = 3.2
rng = np.random.default_rng(7)

def env(n, attack=0.03, release=0.08):
    e = np.ones(n)
    a = max(1, int(attack * SR))
    r = max(1, int(release * SR))
    e[:a] = np.linspace(0, 1, a)
    e[-r:] = np.linspace(1, 0, r)
    return e

def voiced(n, f0, amp):
    t = np.arange(n) / SR
    # time-varying f0: slight declination, like a real syllable
    f = f0 * (1.0 - 0.06 * t / max(t[-1], 1e-6))
    phase = 2 * np.pi * np.cumsum(f) / SR
    sig = np.zeros(n)
    for k, w in enumerate([1.0, 0.55, 0.32, 0.18, 0.10], start=1):
        sig += w * np.sin(k * phase)
    sig /= np.max(np.abs(sig)) + 1e-9
    return amp * sig * env(n)

def quiet(n, amp):
    x = rng.standard_normal(n)
    # low-pass: cumulative smoothing keeps ZCR low, like room hum
    for _ in range(6):
        x = np.convolve(x, np.ones(9) / 9, mode="same")
    x /= np.max(np.abs(x)) + 1e-9
    hum = 0.35 * np.sin(2 * np.pi * 50 * np.arange(n) / SR)
    return amp * (x + hum)

def unvoiced(n, amp, lo, hi):
    x = rng.standard_normal(n)
    # simple FFT band-pass
    X = np.fft.rfft(x)
    freqs = np.fft.rfftfreq(n, 1 / SR)
    mask = (freqs >= lo) & (freqs <= hi)
    X[~mask] = 0
    x = np.fft.irfft(X, n)
    x /= np.max(np.abs(x)) + 1e-9
    return amp * x * env(n, 0.01, 0.02)

t = np.arange(int(DUR * SR)) / SR
sig = quiet(len(t), 0.010)

segments = [
    (0.30, 0.55, "unvoiced", lambda n: unvoiced(n, 0.085, 2500, 7000)),
    (0.55, 1.20, "voiced",   lambda n: voiced(n, 185, 0.55)),
    (1.20, 1.35, "silence",  lambda n: quiet(n, 0.010)),
    (1.35, 2.05, "voiced",   lambda n: voiced(n, 145, 0.45)),
    (2.05, 2.30, "unvoiced", lambda n: unvoiced(n, 0.065, 3000, 7500)),
]
for a, b, kind, gen in segments:
    i0, i1 = int(a * SR), int(b * SR)
    n = i1 - i0
    sig[i0:i1] = gen(n)

def frames(x):
    n = (len(x) - FRAME) // HOP + 1
    return np.stack([x[i * HOP:i * HOP + FRAME] for i in range(n)])

F = frames(sig)
energy = 10 * np.log10(np.mean((F * 32768.0) ** 2, axis=1) + 1e-12)
zcr = np.mean(np.abs(np.diff(np.sign(F), axis=1)) > 0, axis=1)
times = (np.arange(len(energy)) * HOP + FRAME / 2) / SR

# ReferenceVad-style peak-relative thresholds
high = energy.max() - 10.0
low = energy.max() - 20.0
zth = zcr.max() * 0.20
min_voice = 3

# segmentation
segs = []
begin = -1
for i in range(len(energy) + 1):
    above = i < len(energy) and energy[i] > high
    if above and begin < 0:
        begin = i
    if not above and begin >= 0:
        if i - begin >= min_voice:
            segs.append([begin, i - 1])
        begin = -1
for s in segs:
    while s[0] > 0 and energy[s[0] - 1] > low:
        s[0] -= 1
    while s[1] + 1 < len(energy) and energy[s[1] + 1] > low:
        s[1] += 1
    while s[0] > 0 and zcr[s[0] - 1] >= zth:
        s[0] -= 1
    while s[1] + 1 < len(zcr) and zcr[s[1] + 1] >= zth:
        s[1] += 1
merged = []
for s in segs:
    if merged and s[0] <= merged[-1][1] + 1:
        merged[-1][1] = max(merged[-1][1], s[1])
    else:
        merged.append(s)

def shade_states(ax):
    colors = {"silence": "#E8EDF4", "unvoiced": "#FCE8D5", "voiced": "#D8F0EC"}
    labels = {"silence": "无声", "unvoiced": "清音", "voiced": "浊音"}
    spans = [(0.0, 0.30, "silence"), (0.30, 0.55, "unvoiced"),
             (0.55, 1.20, "voiced"), (1.20, 1.35, "silence"),
             (1.35, 2.05, "voiced"), (2.05, 2.30, "unvoiced"),
             (2.30, 3.20, "silence")]
    for a, b, k in spans:
        ax.axvspan(a, b, color=colors[k], alpha=0.75, lw=0, zorder=0)

# ── Figure 1: cover signal + energy ─────────────────────────────────────
fig, (axw, axe) = plt.subplots(2, 1, figsize=(4.6, 2.7), dpi=260, sharex=True,
                               gridspec_kw={"height_ratios": [1.0, 1.25], "hspace": 0.06})
fig.subplots_adjust(0, 0, 1, 1)
for ax in (axw, axe):
    ax.set_facecolor("none")
    for s in merged:
        a = max(0.0, (s[0] * HOP - HOP / 2) / SR)
        b = min(DUR, ((s[1] + 1) * HOP + HOP / 2) / SR)
        ax.axvspan(a, b, color="#2DD4BF", alpha=0.05, lw=0)
        ax.axvline(a, color="#2DD4BF", lw=0.55, alpha=0.55)
        ax.axvline(b, color="#2DD4BF", lw=0.55, alpha=0.55)
    ax.set_xlim(0, DUR)
    ax.axis("off")

axw.plot(t, sig, color="#7DD3FC", lw=0.4, alpha=0.95)
axw.axhline(0, color="#334155", lw=0.5)
axw.set_ylim(-0.72, 0.72)

axe.plot(times, energy, color="#5EEAD4", lw=1.35)
axe.fill_between(times, energy.min() - 2, energy, color="#2DD4BF", alpha=0.07)
axe.axhline(high, color="#FBBF24", lw=0.9, ls=(0, (4, 3)))
axe.axhline(low, color="#FBBF24", lw=0.7, ls=(0, (2, 2)), alpha=0.65)
axe.set_ylim(energy.min() - 3, energy.max() + 3)

fig.savefig(ASSETS / "cover_signal.png", transparent=True, bbox_inches="tight", pad_inches=0.02)
plt.close(fig)

# ── Figure 2: three acoustic states ─────────────────────────────────────
fig, axes = plt.subplots(3, 1, figsize=(6.6, 3.55), dpi=260, sharex=True,
                         gridspec_kw={"height_ratios": [1.25, 1.0, 0.85], "hspace": 0.10})
fig.subplots_adjust(left=0.075, right=0.985, top=0.93, bottom=0.11)

# state strip on top
ax0 = axes[0]
shade_states(ax0)
ax0.plot(t, sig, color=NAVY, lw=0.42, alpha=0.92)
ax0.set_ylim(-0.7, 0.7)
ax0.set_ylabel("波形", fontsize=9.5, color=INK, labelpad=8)
ax0.set_yticks([-0.5, 0, 0.5])
ax0.tick_params(axis="y", labelsize=7.5, colors=MUTED, length=0)
ax0.tick_params(axis="x", bottom=False, labelbottom=False)

# labels above waveform
for a, b, lab, col in [(0.02, 0.28, "无声", MUTED), (0.31, 0.54, "清音", AMBER),
                       (0.58, 1.17, "浊音", TEAL), (1.38, 2.02, "浊音", TEAL),
                       (2.07, 2.28, "清音", AMBER)]:
    ax0.text((a + b) / 2, 0.60, lab, ha="center", va="bottom", fontsize=7.5,
             color=col, fontweight="bold")

ax1 = axes[1]
shade_states(ax1)
ax1.plot(times, energy, color=BLUE, lw=1.15)
ax1.axhline(high, color=ROSE, lw=0.9, ls=(0, (4, 3)))
ax1.axhline(low, color=AMBER, lw=0.9, ls=(0, (2, 2)))
box = dict(boxstyle="round,pad=0.16", fc="#FFFFFF", ec="none", alpha=0.88)
ax1.text(3.17, high + 1.2, "Eh", ha="right", va="bottom", fontsize=7.5,
         color=ROSE, fontweight="bold", bbox=box)
ax1.text(3.17, low - 1.2, "El", ha="right", va="top", fontsize=7.5,
         color=AMBER, fontweight="bold", bbox=box)
ax1.set_ylabel("能量 dB", fontsize=9.5, color=INK, labelpad=8)
ax1.tick_params(axis="y", labelsize=7.5, colors=MUTED, length=0)
ax1.tick_params(axis="x", bottom=False, labelbottom=False)
ax1.set_ylim(min(low - 6, energy.min() - 2), max(energy) + 5)

ax2 = axes[2]
shade_states(ax2)
ax2.plot(times, zcr, color=TEAL, lw=1.05)
ax2.axhline(zth, color=AMBER, lw=0.9, ls=(0, (2, 2)))
ax2.text(3.17, zth + 0.012, "Zth", ha="right", va="bottom", fontsize=7.5,
         color=AMBER, fontweight="bold", bbox=box)
ax2.set_ylabel("ZCR", fontsize=9.5, color=INK, labelpad=8)
ax2.set_xlabel("时间 / s", fontsize=8.5, color=MUTED, labelpad=2)
ax2.tick_params(axis="both", labelsize=7.5, colors=MUTED, length=0)
ax2.set_xlim(0, DUR)
ax2.set_ylim(0, max(zcr) * 1.28)

for ax in axes:
    for sp in ["top", "right", "left", "bottom"]:
        ax.spines[sp].set_visible(False)
    ax.grid(axis="y", color=LINE, lw=0.45, alpha=0.55)
    ax.set_axisbelow(True)

fig.savefig(ASSETS / "feature_states.png", transparent=True, bbox_inches="tight", pad_inches=0.03)
plt.close(fig)

# ── Figure 3: threshold decision ────────────────────────────────────────
fig, axes = plt.subplots(2, 1, figsize=(5.6, 3.55), dpi=260, sharex=True,
                         gridspec_kw={"height_ratios": [1.35, 1.0], "hspace": 0.08})
fig.subplots_adjust(left=0.085, right=0.985, top=0.955, bottom=0.12)

ax = axes[0]
for s in merged:
    a = max(0.0, (s[0] * HOP - HOP / 2) / SR)
    b = min(DUR, ((s[1] + 1) * HOP + HOP / 2) / SR)
    ax.axvspan(a, b, color=TEAL, alpha=0.10, lw=0)
ax.plot(times, energy, color=BLUE, lw=1.3, label="帧能量")
ax.axhline(high, color=ROSE, lw=1.0, ls=(0, (5, 3)))
ax.axhline(low, color=AMBER, lw=1.0, ls=(0, (2, 2)))
ax.text(2.44, high + 1.2, "Eh = Emax − 10 dB", fontsize=7.5, color=ROSE,
        fontweight="bold", va="bottom", bbox=box)
ax.text(2.44, low - 4.5, "El = Emax − 20 dB", fontsize=7.5, color=AMBER,
        fontweight="bold", va="top", bbox=box)
ax.set_ylabel("能量 / dB", fontsize=9, color=INK)
ax.tick_params(axis="y", labelsize=7.5, colors=MUTED, length=0)
ax.tick_params(axis="x", bottom=False, labelbottom=False)
ax.set_ylim(min(low - 6, energy.min() - 2), max(energy) + 5)
ax.set_title("高门限锁定核心，低门限向两侧扩展", loc="left", fontsize=9.5,
             color=NAVY, fontweight="bold", pad=4)

ax = axes[1]
for s in merged:
    a = max(0.0, (s[0] * HOP - HOP / 2) / SR)
    b = min(DUR, ((s[1] + 1) * HOP + HOP / 2) / SR)
    ax.axvspan(a, b, color=TEAL, alpha=0.10, lw=0)
ax.plot(times, zcr, color=TEAL, lw=1.2, label="过零率")
ax.axhline(zth, color=AMBER, lw=1.0, ls=(0, (2, 2)))
ax.text(2.44, zth + 0.03, "Zth = 0.2 × ZCRmax", fontsize=7.5, color=AMBER,
        fontweight="bold", va="bottom", bbox=box)
ax.set_ylabel("ZCR", fontsize=9, color=INK)
ax.set_xlabel("时间 / s", fontsize=8.5, color=MUTED, labelpad=2)
ax.tick_params(axis="both", labelsize=7.5, colors=MUTED, length=0)
ax.set_xlim(0, DUR)
ax.set_ylim(0, max(zcr) * 1.28)
ax.set_title("过零率沿边界二次搜寻清辅音", loc="left", fontsize=9.5,
             color=NAVY, fontweight="bold", pad=4)

for ax in axes:
    for sp in ["top", "right", "left", "bottom"]:
        ax.spines[sp].set_visible(False)
    ax.grid(axis="y", color=LINE, lw=0.45, alpha=0.55)
    ax.set_axisbelow(True)

fig.savefig(ASSETS / "vad_threshold.png", transparent=True, bbox_inches="tight", pad_inches=0.03)
plt.close(fig)

# ── Figure 4: compact three-lane UI preview ─────────────────────────────
fig, axes = plt.subplots(3, 1, figsize=(3.9, 1.55), dpi=300, sharex=True,
                         gridspec_kw={"hspace": 0.02})
fig.subplots_adjust(0, 0, 1, 1)
for ax in axes:
    ax.set_facecolor("none")
    for s in merged:
        a = max(0.0, (s[0] * HOP - HOP / 2) / SR)
        b = min(DUR, ((s[1] + 1) * HOP + HOP / 2) / SR)
        ax.axvspan(a, b, color=TEAL, alpha=0.12, lw=0)
    ax.axvline(1.55, color=ROSE, lw=0.8, ls=(0, (2, 2)), alpha=0.9)
    ax.set_xlim(0, DUR)
    ax.axis("off")

win = max(1, int(0.004 * SR))
nwin = len(sig) // win
rms = np.sqrt(np.mean(sig[:nwin * win].reshape(nwin, win) ** 2, axis=1))
tr = (np.arange(nwin) * win + win / 2) / SR
axes[0].fill_between(tr, -rms, rms, color=NAVY, alpha=0.55, lw=0)
axes[0].plot(tr, rms, color=NAVY, lw=0.45)
axes[0].set_ylim(-0.68, 0.68)
axes[1].plot(times, energy, color=BLUE, lw=0.9)
axes[1].set_ylim(energy.min() - 2, energy.max() + 3)
axes[2].plot(times, zcr, color=TEAL, lw=0.9)
axes[2].set_ylim(0, zcr.max() * 1.15)

fig.savefig(ASSETS / "ui_lanes.png", transparent=True, bbox_inches="tight", pad_inches=0)
plt.close(fig)

print("segments:", merged)
print("thresholds:", high, low, zth)
print("figures written to ppt/assets")
