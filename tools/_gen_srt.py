
# -*- coding: utf-8 -*-
"""用电影字幕为「电影片段类」过场动画生成中文字幕"""
import json, os, re

subs = json.load(open(r"D:\Documents\DSH\bf1_chs\movie_subs_all.json", encoding="utf-8"))
USE = [k for k in subs if any(t in k for t in ("幽灵的威胁","克隆人的进攻","新希望","帝国反击战","绝地归来"))]
ASR = r"D:\Documents\DSH\bf1_chs\asr2"
OUTD = r"D:\Documents\DSH\bf1_chs\subs_out"
os.makedirs(OUTD, exist_ok=True)

def norm(s):
    s = s.lower(); s = re.sub(r"[^a-z0-9 ]", " ", s)
    return [w for w in s.split() if w]

def cover(a, b):
    A, B = set(norm(a)), set(norm(b))
    if not A: return 0.0
    return len(A & B) / len(A)

def dur_of(clip):
    import wave
    p = os.path.join(r"D:\Documents\DSH\bf1_chs\audio", clip + ".wav")
    with wave.open(p, "rb") as w:
        return w.getnframes() / w.getframerate()

def srt_time(t):
    h = int(t // 3600); m = int((t % 3600) // 60); s = t % 60
    return "%02d:%02d:%06.3f" % (h, m, s)

for f in sorted(os.listdir(ASR)):
    clip = f[:-5]
    segs = json.load(open(os.path.join(ASR, f), encoding="utf-8"))
    duration = dur_of(clip)
    anchors = []
    for s in segs:
        w = norm(s["text"])
        if len(w) < 4: continue
        for movie in USE:
            for (t1, t2, txt) in subs[movie]:
                c = cover(s["text"], txt)
                if c >= 0.8 and len(norm(txt)) >= 4:
                    anchors.append((c, movie, t1, s["start"], txt))
    if not anchors:
        print("%-9s 无可靠锚点 (segs=%d)" % (clip, len(segs))); continue
    # 找票数最多的 (movie, offset)
    from collections import Counter
    votes = Counter()
    for c, movie, t1, st, txt in anchors:
        votes[(movie, round((t1 - st) / 2.0) * 2)] += 1
    (movie, off), n = votes.most_common(1)[0]
    if n < 1:
        print("%-9s 锚点不足" % clip); continue
    def keep(txt):
        if "━" in txt or "─" in txt or "■" in txt: return False
        if not any(("a" <= ch.lower() <= "z") for ch in txt): return False   # 注释条多为纯中文
        return True
    lines = [(t1 - off, t2 - off, txt) for (t1, t2, txt) in subs[movie]
             if 0 <= t1 - off <= duration and keep(txt)]
    # 去重（相邻相同文本）
    dedup = []
    for a, b, txt in lines:
        if dedup and dedup[-1][2] == txt and a - dedup[-1][0] < 1.0: continue
        dedup.append((a, b, txt))
    lines = dedup
    lines.sort()
    if not lines:
        print("%-9s 窗口内无字幕" % clip); continue
    path = os.path.join(OUTD, clip + ".srt")
    with open(path, "w", encoding="utf-8-sig") as fh:
        for i, (a, b, txt) in enumerate(lines, 1):
            fh.write("%d\n%s --> %s\n%s\n\n" % (i, srt_time(a), srt_time(b), txt))
    print("%-9s -> %s  偏移=%.1fs 锚点=%d 行数=%d" % (clip, movie[:12], off, n, len(lines)))