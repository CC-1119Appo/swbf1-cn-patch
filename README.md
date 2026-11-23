# 星球大战：前线（2004）简体中文补丁

Star Wars: Battlefront (2004) Simplified Chinese Patch

适用于 **GOG 版 1.2 / 1.3**（Steam 版理论通用，未测试）。

## 包含内容

| 模块 | 说明 |
|---|---|
| 界面与文本 | 2118 条文本汉化：菜单、HUD、任务简报、多人界面、兵种/武器说明 |
| 字体 | 6 套字体全部重建，1077 个汉字，无缺字 |
| 过场动画 | 11 段影片重编码并烧入中英对照字幕（画质高于原版） |
| 术语 | 参照 19352 条星战中英对照表统一译名 |
| 安装器 | Win32 图形安装器，支持备份/还原、安装范围选择，兼容 XP SP3+ |

## 安装

1. 到 Releases 下载安装包并解压；
2. 运行 `安装_汉化包.exe`；
3. 程序自动搜索 GOG / Steam / LucasArts 常见路径，也可以手动选择 `GameData` 目录；
4. 点「检测版本」确认状态，选择安装范围，点「开始安装」。

安装前默认备份原版：

```text
data/_lvl_pc/core.lvl                -> core.lvl.original
data/_lvl_pc/movies/shell.mvs        -> shell.mvs.original
```

## 版本兼容

安装器会读取现有 `core.lvl` 的大小判断版本，自动选择对应载荷：

| 原版 core.lvl 大小 | 版本 | 使用的载荷 |
|---|---|---|
| 4,895,408 | GOG 1.2 | payload/core.lvl.12 |
| 4,899,596 | GOG 1.3 | payload/core.lvl |

## 从源码构建

1. 安装 Python 3.10+ 与 Pillow；
2. 准备字体 `C:/Windows/Fonts/simhei.ttf`；
3. 用自己机器上的原版 `core.lvl` 作基底：

```bash
python tools/build_bf1_cjk.py path/to/core.lvl build/core_cn.lvl data/workbook.zh.json
```

4. 影片字幕（需要 RAD Video Tools 与 ffmpeg）：

```bash
python tools/_bink_best.py          # 用默认最高质量编码 Bink
python tools/_mvs_embed3.py         # 替换进 shell.mvs 容器
```

5. 编译安装器：

```bat
installer\build_xp.bat
```

## 目录结构

```text
data/workbook.zh.json      译文表（id -> 中文）
docs/                      安装说明、开发笔记、审计报告
installer/                 Win32 安装器源码
tools/                     构建脚本（文本/字体/影片/字幕）
screenshots/               截图
```

## 已知差异

- HUD 上的数字（血量/弹药/增援）是原版贴图，不随汉化改变；
- 操作界面顶部三个分类标签的横向位置由引擎硬编码，未调整；
- 游戏内对白字幕需在「选项 → 音频选项 → 字幕」手动开启；
- 剩余英文仅法律声明与 PS2/Xbox 专属提示，PC 版不可见（见 docs/AUDIT_RESIDUAL_EN.md）。

## 致谢与声明

- 游戏版权归 LucasArts / Disney 所有，本补丁仅包含汉化文本、字体与重编码影片；
- 使用本补丁需要拥有正版游戏；
- 代码以 MIT 协议开源，见 LICENSE；
- 引擎文件格式解析参考了同引擎的《前线2》汉化项目。
