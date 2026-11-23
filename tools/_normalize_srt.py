
# -*- coding: utf-8 -*-
"""规范化 SRT：单 BOM、逗号毫秒、LF 换行"""
import os, re
DIRS = [r"C:\Users\Kille\Desktop\BF1_过场动画\字幕",
        r"C:\Users\Kille\Desktop\BF1_过场动画\MP4"]
def norm_time(t):
    t = t.strip().replace(".", ",")
    return t
for d in DIRS:
    for f in os.listdir(d):
        if not f.endswith(".srt"): continue
        p = os.path.join(d, f)
        raw = open(p, "rb").read().decode("utf-8-sig", "replace")
        raw = raw.replace("\ufeff", "").replace("\r\n", "\n").replace("\r", "\n")
        blocks = [b for b in raw.strip().split("\n\n") if b.strip()]
        out = []
        for i, b in enumerate(blocks, 1):
            lines = [l for l in b.split("\n") if l.strip()]
            if len(lines) < 2: continue
            tm = lines[0] if "-->" in lines[0] else lines[1]
            text = lines[1:] if "-->" in lines[0] else lines[2:]
            a, bb = tm.split("-->")
            out.append("%d\n%s --> %s\n%s" % (i, norm_time(a), norm_time(bb), "\n".join(text)))
        content = "\n\n".join(out) + "\n"
        open(p, "w", encoding="utf-8", newline="\n").write(content)
    print("normalized:", d)
p = r"C:\Users\Kille\Desktop\BF1_过场动画\字幕\shell_40.srt"
print(repr(open(p, "rb").read()[:110]))
