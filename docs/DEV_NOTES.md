# 开发笔记

## 1. 文本存储

游戏文本全部在 `Data/_LVL_PC/core.lvl` 的 `Locl` 块中（6 种语言：english/spanish/italian/french/german/uk_english）。
其余 147 个 lvl 文件均无文本块；mission.lvl 只有 44 段 Lua 任务脚本。

条目结构：id（32 位哈希）+ UTF-16LE 文本。译文表见 `data/workbook.zh.json`。

## 2. 字符编码：代理码位

引擎字体表只有 16 位码位，且部分汉字码位 >= 0x8000 会被截断。
做法：把所有 >= 0x8000 的汉字按顺序重新映射到 0x0100 起的空档（0x0100~0x0FFF 区间未被使用），
写入文本时替换，字体位图也按新码位生成，游戏内显示即正常。

当前 1077 个汉字中有 276 个走代理码位。

## 3. 字体重建

每套字体 = HEAD + FTEX（贴图，A4R4G4B4）+ FBOD（字形表，24 字节/条）。
用 Pillow 渲染 SimHei，写入贴图并更新字形框；6 套字体的字号：

| 字体 | 字号 | 贴图 |
|---|---|---|
| gamefont_large | 22 | 2048x2048 |
| gamefont_medium | 19 | 1024x1024 |
| gamefont_small | 16 | 1024x1024 |
| gamefont_tiny | 18 | 1024x1024 |
| gamefont_super_tiny | 17 | 1024x1024 |
| starwars_small | 14 | 1024x1024 |

## 4. 影片容器 shell.mvs

结构（已通过原样重建逐字节校验）：

```text
[2048 字节头部]
  起始 48，55 个 32 字节条目：
    +20 影片大小 + 8
    +28 附加块大小 - 8（附加块 = 16 字节头 + 数据）
[55 段 Bink 影片，逐个紧密排列]
```

替换影片时重算大小字段即可，偏移由累加得出。

## 5. 字幕烧录流程

1. 从 shell.mvs 拆出 55 段 Bink；
2. ffmpeg 转 MP4；
3. Whisper 识别有对白的 9 段 + 2 段 crawl；
4. 人工润色生成中英双语 SRT（中文在上、英文在下）；
5. ffmpeg 烧入字幕（ASS 样式，底部居中）；
6. RAD Video Tools `Binkc` 重编码，**不加 /D 参数**（默认最高质量，体积高于原片）；
7. 回填 shell.mvs。

注意：Binkc 对含中文的路径会失败，需先复制到纯 ASCII 路径。

## 6. 安装器

纯 Win32 API，无 .NET。
字体用默认控件字体（中文系统即宋体），不加粗、不设 DPI 清单，行为与同引擎的《前线2》安装器一致。
载荷放在 exe 同目录 `payload/`，安装时流式复制。
