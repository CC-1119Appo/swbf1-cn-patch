
# -*- coding: utf-8 -*-
"""用权威中文 + 电影英文生成中英对照 SRT"""
import json, os, re, wave

ZH = json.load(open(r"D:\Documents\DSH\bf1_chs\zh_lines.json", encoding="utf-8"))
subs = json.load(open(r"D:\Documents\DSH\bf1_chs\movie_subs_all.json", encoding="utf-8"))
NAME = {1: "星球大战1：幽灵的威胁 (1999).ass", 2: "星球大战2：克隆人的进攻 (2002).ass",
        4: "星球大战4：新希望 (1977).ass", 5: "星球大战5：帝国反击战 (1980).ass"}
ANCH = {"shell_33": (5, 6442.1), "shell_36": (2, 7100.6), "shell_37": (5, 1479.5),
        "shell_38": (2, 8014.0), "shell_41": (1, 6420.7), "shell_43": (4, 3288.0),
        "shell_44": (4, 3535.0), "shell_45": (4, 5791.6), "shell_46": (5, 5749.3)}
DIR = r"C:\Users\Kille\Desktop\BF1_过场动画\字幕"
DIR2 = r"C:\Users\Kille\Desktop\BF1_过场动画\MP4"

def en_of(txt):
    t = re.sub(r"[\u4e00-\u9fff]", " ", txt)
    t = re.sub(r"[━─■]", " ", t)
    t = re.sub(r"\s{2,}", " ", t)
    t = re.sub(r"^[\s\-—?？!！.。,，:：;；]+", "", t)
    t = re.sub(r"[\s\-—?？!！.。,，:：;；]+$", "", t)
    return t.strip()

def dur(clip):
    with wave.open(os.path.join(r"D:\Documents\DSH\bf1_chs\audio", clip + ".wav"), "rb") as w:
        return w.getnframes() / w.getframerate()

def sec(t):
    h, m, s = t.replace(",", ".").split(":")
    return int(h)*3600 + int(m)*60 + float(s)

def fmt(t):
    h = int(t // 3600); m = int((t % 3600) // 60); s = t % 60
    return ("%02d:%02d:%06.3f" % (h, m, s)).replace(".", ",")

for clip, (mno, off) in ANCH.items():
    src = os.path.join(DIR, clip + ".srt")
    times = []
    for b in open(src, encoding="utf-8-sig").read().strip().split("\n\n"):
        ls = b.split("\n")
        if len(ls) < 3: continue
        a, bb = ls[1].split("-->")
        times.append((sec(a), sec(bb)))
    zh = ZH[clip]
    assert len(times) == len(zh), (clip, len(times), len(zh))
    d = dur(clip)
    film = [(a - off, en_of(t)) for (a, b, t) in subs[NAME[mno]]
            if -0.5 <= a - off <= d and len(en_of(t)) >= 2]
    buckets = [[] for _ in times]
    for t, e in film:
        best, bd = None, 1e9
        for i, (a, b) in enumerate(times):
            dd = abs(t - a)
            if dd < bd: best, bd = i, dd
        if best is not None and bd <= 3.0:
            buckets[best].append((t, e))
    out = []
    for i, ((a, b), z) in enumerate(zip(times, zh), 1):
        en = " ".join(e for _, e in sorted(buckets[i-1])).strip()
        en = re.sub(r"\s{2,}", " ", en)
        out.append("%d\n%s --> %s\n%s" % (i, fmt(a), fmt(b), z + ("\n" + en if en else "")))
    body = "\n\n".join(out) + "\n"
    for tgt in (src, os.path.join(DIR2, clip + ".srt")):
        open(tgt, "w", encoding="utf-8", newline="\n").write(body)
    print("%-9s OK %d 行" % (clip, len(out)))
