# code/ —— 黑白棋 capstone 靶场（可选）

> 主计划是 [`../02_plan90.md`](../02_plan90.md) 的 8 个题型族 + 冷解。
> 这个目录只在「九坤考博弈/实现大题」或你想做一个能讲深的项目时用；不考就跳过。

```
spec.md     训练题面与 I/O（先读）
oracle.py   精确解算器（小棋盘 ground truth）
stub.cpp    你的骨架：只需要填 genMoves / negamax
gen.py      随机用例生成
stress.py   对拍脚本
Makefile    make / make asan / make tsan
```

## 5 分钟跑通

```bash
cd code
python3 oracle.py --selftest          # 先确认 oracle 自己是好的
make                                  # 编译 stub
printf '3 3\n3 3\n...\n.b.\n.w.\n' > /tmp/c.txt
python3 oracle.py --file /tmp/c.txt --stats   # BLACK
./stub < /tmp/c.txt                           # 现在固定输出 DRAW（未实现）
python3 stress.py --bin ./stub --count 10     # 全红，正常
```

## 你的实现顺序

1. **M1**：`genMoves`。写完先跑 `stress.py --moves`，要求 200 组用例
   （3x3~4x4，1~2 子，随机障碍、随机 k）全部一致。
2. **M2**：`negamax` 不带剪枝、不带 TT，固定深度或直接搜到终局。
   在小棋盘（3x3、1 子）上用 `stress.py` 对结果；深度给到 100+，并加重复检测（否则循环会无限递归）。
3. **M3**：加 Zobrist + `unordered_map` 置换表，记录节点数。
4. **M4**：alpha-beta + move ordering（win 走法优先、吃子优先、history）。
5. **M5**：位棋盘 + 增量哈希 + 迭代加深（`stub.cpp` 的 Mask 已经够用，
   优化的是“怎么少算”而不是“换一种存法”）。
6. **M6**：ASan/UBSan 清零、随机 fuzz、10x10 压测、测试用例文档。

## 三条纪律

- **先对拍，再优化**：每一步都要有“改之前/改之后结果一致 + 节点数下降”的记录。
- **精确 vs 限深**：`stress.py` 的结果模式要求精确搜索；限深版本只用
  `--moves` 对规则，别拿限深结果去和 oracle 比胜负。
- **不要改 oracle 来迁就你的 bug**。oracle 的 `--selftest` 是两套独立算法互验，
  它错了的概率远小于你的第一版。
