# Anki 使用说明

## 手机（AnkiDroid）：直接导入 `.apkg`

AnkiDroid 对 CSV 的支持不稳定，**不要直接导 `deck.csv`**。用已经生成好的：

- 文件：[`QD-CPP.apkg`](QD-CPP.apkg)（134 张，牌组 `QD-C++`）

步骤：

1. 把 `QD-CPP.apkg` 传到手机（微信文件传输 / USB / 网盘均可）。
2. 在文件管理器里点它，选「用 AnkiDroid 打开」；或打开 AnkiDroid → 右上角 `⋮` → `Import` → 选文件。
3. 导入后牌组名是 `QD-C++`，标签保留。
4. 如果提示失败：确认文件后缀是 `.apkg`（不是 `.zip`），并把 AnkiDroid 更新到较新版本。

## 电脑（Anki Desktop）：用 CSV 或 apkg

- 想编辑/加卡：导 `deck.csv`（`File → Import`，分隔符 Comma，字段 1/2/3 → Front/Back/Tags）。
- 想直接同步到手机：导 `QD-CPP.apkg`，再开 AnkiWeb 同步，AnkiDroid 登录同一账号即可。

CSV 文件头三行是**桌面 Anki 的导入指令**，AnkiDroid 不一定认：

```
#separator:Comma
#html:true
#tags column:3
```

## 改完 CSV 后，重新生成 apkg

```bash
pip install genanki
python3 anki/make_apkg.py
# 或：python3 anki/make_apkg.py --csv anki/deck.csv --out anki/QD-CPP.apkg
```

脚本会覆盖旧的 `QD-CPP.apkg`，然后重新传到手机导入即可。

## 每天怎么刷

| 场景 | 设置 |
|---|---|
| 通勤（周一/周二/周三） | 到期卡全部 + 新卡 10 张 |
| 睡前 10 分钟 | 只刷 `transfer` + `cpp` |
| 周六 | 清到期卡，不学新卡 |
| 考前 3 天 | 只刷 `cpp` + `os` + `net` |

**出声读**。默读只能形成再认，出声才能形成回忆。卡住超过 20 秒就看答案，标记 `Again`。

## 卡片分类

| 标签 | 数量 | 用途 |
|---|---|---|
| `transfer` | 16 | 8 个题型族、冷解协议、错题分类 |
| `othello rules` | 18 | 题面/规则/歧义 |
| `othello bits` | 6 | 位棋盘/哈希 |
| `search` | 25 | minimax/alpha-beta/TT/复杂度 |
| `cpp` | 21 | 对象模型/现代 C++ |
| `os` | 20 | 进程线程/内存/容器 |
| `net` | 9 | TCP/epoll/RDMA/DPDK |
| `quant` | 8 | 期货/期权/概率/GIL |
| `hand` | 11 | 手撕题要点 |

> 共 134 张。前 8 周重点刷 `transfer` + `search`，W9 后加 `cpp`/`os`/`net`。

## 自己加卡的三条规则

1. **一张卡只问一个事实**。不要「alpha-beta 的原理、复杂度、剪枝条件是什么」。
2. **用自己的话**。不要抄文档；抄来的卡你会在第二次复习时卡住。
3. **加「为什么」卡**。事实卡（TT 存 depth）+ 原因卡（浅层不能证明深层）成对出现。

## 每周维护

- 周日把本周新遇到的 bug/追问做成 3–5 张卡。
- 把连续 3 次 `Easy` 的卡删掉或合并。
- 卡片总数控制在 200 以内；超过说明你在收集，不是在记忆。
