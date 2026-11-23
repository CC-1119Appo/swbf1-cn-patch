
# -*- coding: utf-8 -*-
"""用编码好的 *_hq.bik 替换进 shell.mvs"""
import os, struct

SRC = r"E:\GOG Games\Star Wars - Battlefront\GameData\Data\_LVL_PC\Movies\shell.mvs.original"
WORK = r"D:\Documents\DSH\bf1_chs\bink"
OUT = r"D:\Documents\DSH\bf1_chs\bink\shell_cn.mvs"
HEAD, N, BASE = 2048, 55, 48

TARGETS = {"shell_33": 32, "shell_36": 35, "shell_37": 36, "shell_38": 37, "shell_40": 39,
           "shell_41": 40, "shell_42": 41, "shell_43": 42, "shell_44": 43, "shell_45": 44, "shell_46": 45}

d = open(SRC, "rb").read()
head = bytearray(d[:HEAD])
ents = [list(struct.unpack_from("<8I", head, BASE + i*32)) for i in range(N)]
movies, off = [], HEAD
for i in range(N):
    size = ents[i][5] - 8
    aux = ents[i][7] + 8
    movies.append([d[off:off+size], d[off+size:off+size+aux]])
    off += size + aux

for clip, idx in TARGETS.items():
    p = os.path.join(WORK, clip + "_best.bik")
    if not os.path.exists(p):
        print("缺少", clip, "- 跳过"); continue
    data = bytearray(open(p, "rb").read())
    struct.pack_into("<I", data, 4, len(data) - 8)
    old = len(movies[idx][0])
    movies[idx][0] = bytes(data)
    print("%-9s #%2d  %6.2f MB -> %6.2f MB" % (clip, idx, old/1048576, len(data)/1048576))

body = bytearray()
for i, (vid, aux) in enumerate(movies):
    ents[i][5] = len(vid) + 8
    ents[i][7] = len(aux) - 8
    struct.pack_into("<8I", head, BASE + i*32, *ents[i])
    body += vid + aux
total = HEAD + len(body)
struct.pack_into("<I", head, 4, total - 8)
struct.pack_into("<I", head, 12, total - 16)
struct.pack_into("<I", head, 20, total - 24)
open(OUT, "wb").write(bytes(head) + bytes(body))
print("输出 %s  %.1f MB" % (OUT, total/1048576))