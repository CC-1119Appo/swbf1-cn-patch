
# -*- coding: utf-8 -*-
"""用默认（最高）质量重新编码 11 段字幕影片"""
import os, subprocess, time

RAD = r"D:\RADVideo\radvideo64.exe"
WORK = r"D:\Documents\DSH\bf1_chs\bink"
NAMES = ["shell_33","shell_36","shell_37","shell_38","shell_41","shell_43","shell_44","shell_45","shell_46","shell_40","shell_42"]
subprocess.run(["taskkill","/F","/IM","radvideo64.exe"], capture_output=True)
time.sleep(1)
for n in NAMES:
    src = os.path.join(WORK, n + ".mp4")
    dst = os.path.join(WORK, n + "_best.bik")
    if os.path.exists(dst): os.remove(dst)
    p = subprocess.Popen([RAD, "Binkc", src, dst])
    t0 = time.time(); last = -1
    while time.time() - t0 < 600:
        if os.path.exists(dst):
            sz = os.path.getsize(dst)
            if sz == last and sz > 1000: break
            last = sz
        if p.poll() is not None and not os.path.exists(dst): break
        time.sleep(1.5)
    try: p.kill()
    except Exception: pass
    mb = os.path.getsize(dst)/1048576 if os.path.exists(dst) else 0
    print("%-9s %6.2f MB  %.0fs" % (n, mb, time.time()-t0), flush=True)
print("DONE")
