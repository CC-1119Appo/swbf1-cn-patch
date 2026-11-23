
# -*- coding: utf-8 -*-
"""重建 shell.mvs：支持替换部分影片，保持头部结构"""
import struct, os, sys

SRC = r"E:\GOG Games\Star Wars - Battlefront\GameData\Data\_LVL_PC\Movies\shell.mvs.original"
HEAD, N, BASE = 2048, 55, 48

def load(path):
    d = open(path, "rb").read()
    head = bytearray(d[:HEAD])
    ents = [list(struct.unpack_from("<8I", head, BASE + i*32)) for i in range(N)]
    movies, off = [], HEAD
    for i in range(N):
        size = ents[i][5] - 8
        aux = ents[i][7] + 8
        movies.append((d[off:off+size], d[off+size:off+size+aux]))
        off += size + aux
    return head, ents, movies

def build(head, ents, movies, out):
    head = bytearray(head)
    body = bytearray()
    off = HEAD
    for i, (vid, aux) in enumerate(movies):
        ents[i][5] = len(vid) + 8
        ents[i][7] = len(aux) - 8
        struct.pack_into("<8I", head, BASE + i*32, *ents[i])
        body += vid
        body += aux
        off += len(vid) + len(aux)
    total = HEAD + len(body)
    struct.pack_into("<I", head, 4, total - 8)
    struct.pack_into("<I", head, 12, total - 16)
    struct.pack_into("<I", head, 20, total - 24)
    open(out, "wb").write(bytes(head) + bytes(body))
    return total

head, ents, movies = load(SRC)
out = r"D:\Documents\DSH\bf1_chs\bink\identity.mvs"
t = build(head, ents, movies, out)
same = open(SRC, "rb").read() == open(out, "rb").read()
print("重建大小", t, "原文件", os.path.getsize(SRC), "字节一致:", same)
