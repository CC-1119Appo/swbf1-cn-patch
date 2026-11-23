# 发布资源说明

仓库内不含游戏数据（体积与版权原因）。完整安装包包含：

```text
SWBF1_汉化包_v1.0.zip
  安装_汉化包.exe        约 96 KB
  README.md
  payload/core.lvl       约 25.1 MB（1.3）
  payload/core.lvl.12    约 25.1 MB（1.2）
  payload/shell.mvs      约 869.7 MB
```

## 自行构建 payload

1. 从正版游戏复制原版 `core.lvl`；
2. `python tools/build_bf1_cjk.py <原版 core.lvl> payload/core.lvl data/workbook.zh.json`；
3. 1.2 版用 1.2 的 core.lvl 重复一次，输出 `payload/core.lvl.12`；
4. 影片：`python tools/_bink_best.py` 与 `python tools/_mvs_embed3.py`；
5. 编译安装器后连同 payload 一起打包。
