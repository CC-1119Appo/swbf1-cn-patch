# 残留英文审计报告（mission.lvl / 全游戏 lvl）

日期：2026-11-23

## 一、结论

**mission.lvl 内没有任何可翻译文本**。任务目标/简报文字全部存放在 core.lvl 的 Locl 语言块里，
这些文本早已全部汉化；mission.lvl 只包含 44 段 Lua 任务脚本（逻辑），不含显示文本。

## 二、mission.lvl 结构

- 文件：Data/_LVL_PC/mission.lvl
- 顶层块：44 个 lvl_ 子包，每个包一个 scr_ 脚本（NAME/INFO/BODY，Lua 5.0 字节码）
- Locl 文本块：0 个
- 全文可打印英文短语：仅 2 个，且都不是显示文本：Naboo Guard（脚本内单位标识）、scr_（块类型名）

## 三、全游戏扫描（147 个 .lvl）

- 扫描 Data/_LVL_PC 下全部 147 个 .lvl 的块表
- 含 Locl 文本块的文件：只有 core.lvl（english / spanish / italian / french / german / uk_english 共 6 块）
- 地图 lvl（bes/tat/hot/kas/nab/geo/rhn/yav/end/kam/side 等）一律没有文本块

## 四、core.lvl english 条目统计

- english 条目总数：2461
- 已含中文（已汉化）：2118（86.1%）
- 仍为英文：343
    - 授权/版权声明（zlib、expat、libpng、LucasArts）：10 条，法律文本，按惯例保留原文
    - 主机存档提示（PS2 记忆卡 / Xbox 硬盘）：60 条，PC 版不可见
    - 按键名与符号（Esc/Tab/A/B/[] 等）：82 条，无法翻译
    - PS2 网络认证（DNAS）与网络错误码：45 条，PC 版不可见

## 五、任务目标/简报抽样（均已汉化）

- id 2120531025：指挥点
- id 2911587979：目标Ȓ
- id 335845069：指挥点
- id 1144102766：你占Ǵ的指挥点数ƺ
- id 1250284516：用于区分目标是敌是友
- id 3614119472：ǫƕ指挥点即可占Ǵ
- id 3273307088：摧毁Ŝ指挥点
- id 969487403：Ɨ是指挥点
- id 2022926653：ǯƯ显示增援数
- id 313794389：3 个奖励目标
- id 495767156：%s ļ摧毁
- id 607132640：ƣ择摧毁星球
- id 3394997801：%sȑƣ择ł摧毁的星球
- id 3867732661：没有可ƣ目标。
- id 990100908：增援Ȓ%s

## 六、建议

1. 不需要修改 mission.lvl；
2. core.lvl 中剩余英文均为法律文本或仅主机版可见的提示，PC 版不会出现；
3. 如需洁癖式完整，可把 45 条 DNAS/网络错误码也译成中文（PC 不可见，仅装饰）。
