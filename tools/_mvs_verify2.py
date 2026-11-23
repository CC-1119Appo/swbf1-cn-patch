
# -*- coding: utf-8 -*-
import struct
P = r"E:\GOG Games\Star Wars - Battlefront\GameData\Data\_LVL_PC\Movies\shell.mvs"
d = open(P, "rb").read()
HEAD, N, BASE = 2048, 55, 48
ents = [list(struct.unpack_from("<8I", d, BASE + i*32)) for i in range(N)]
off = HEAD
ok = True
print("%-3s %-10s %-10s %s" % ("#", "offset", "size", "bink?"))
for i in range(N):
    size = ents[i][5] - 8
    aux = ents[i][7] + 8
    sig = d[off:off+3]
    if sig not in (b"BIK", b"KB2", b"BIN"):
        ok = False
        print("%-3d %-10d %-10d 非 Bink: %r" % (i, off, size, sig))
    if i < 5 or i in (32, 35, 36, 37, 39, 40, 41, 42, 43, 44, 45):
        print("%-3d %-10d %-10d %s" % (i, off, size, sig.decode("latin1")))
    off += size + aux
print("结束位置", off, "文件大小", len(d), "一致:", off == len(d))
print("头部大小字段", struct.unpack_from("<I", d, 4)[0] + 8 == len(d))
