# code/tools.md —— 非科班也要会用的 6 个工具

> 这些工具的价值：把「我觉得是这里错了」变成「我知道是第 37 行错了」。
> 面试时你说「我用 ASan 定位了一个越界」，比说「我调了很久」有说服力。

---

## 1. 编译：三套配置

```bash
# ① 日常：优化 + 警告（提交/压测用）
g++ -O2 -g -std=c++17 -Wall -Wextra -Wshadow stub.cpp -o stub

# ② 调试：关优化 + 断言 + 迭代器检查（慢，但报错早）
g++ -O0 -g -std=c++17 -D_GLIBCXX_DEBUG stub.cpp -o stub_dbg

# ③ 查错：ASan + UBSan（内存越界/未定义行为）
g++ -O1 -g -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer stub.cpp -o stub_asan
```

> `-Wall -Wextra` 的警告不要忽略。`-Wshadow` 能抓出「内层变量遮蔽外层」这种阴间 bug。

---

## 2. gdb：定位崩溃/死循环

```bash
gdb ./stub_dbg
(gdb) run < case.txt
# 崩溃后：
(gdb) bt                 # 调用栈
(gdb) frame 2            # 切到第 2 帧
(gdb) p s                # 打印变量
(gdb) p/x s.b.v[0]       # 十六进制看位图
(gdb) info locals        # 所有局部变量

# 断点：
(gdb) b stub.cpp:87
(gdb) b genMoves
(gdb) c                  # continue
(gdb) n / s              # next / step
(gdb) watch -l s.turn    # 观察变量变化

# 死循环：Ctrl-C，然后 bt 看卡在哪
```

小技巧：

- `p *moves._M_impl._M_start@moves.size()` 打印 vector 内容（或用 `call` 写个打印函数）。
- 编译时加 `-g`，否则看不到行号。
- `ulimit -c unlimited` + `gdb ./stub core` 可以事后调试 core dump。

---

## 3. ASan / UBSan：让越界立刻现形

```bash
./stub_asan < case.txt
```

常见输出：

- `heap-buffer-overflow`：数组/vector 越界；
- `stack-buffer-overflow`：局部数组越界；
- `use-after-free`：悬垂指针；
- `signed integer overflow`：有符号溢出（UBSan）；
- `runtime error: load of misaligned address`：对齐问题。

加 `ASAN_OPTIONS=detect_leaks=1` 查内存泄漏（程序退出时）。

> 你的程序如果「跑小样例对、跑随机数据崩」，先上 ASan，90% 是越界/未初始化。

---

## 4. perf：找出慢在哪

```bash
# 总体统计
perf stat -e cycles,instructions,cache-references,cache-misses,branch-misses ./stub < big.txt

# 采样 + 火焰图（需要 root 或 perf_event_paranoid 调整）
perf record -g ./stub < big.txt
perf report
```

看三个数：

- `cache-misses / cache-references`：缓存命中率；
- `branch-misses`：分支预测失败；
- `instructions / cycles`：IPC，太低说明在等内存/分支。

> 但先做「算法级」优化（剪枝/TT），再考虑「常数级」（位棋盘/cache）。

---

## 5. 计时：不要用 `clock()` 糊弄

```cpp
#include <chrono>
using Clock = std::chrono::steady_clock;

auto t0 = Clock::now();
// ... 被测代码 ...
auto ms = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
```

- `steady_clock` 单调，适合测耗时；`system_clock` 会被校时。
- 测多次取中位数；第一次跑有 cache 冷启动。
- 搜索里每 1024 个节点检查一次 `now()`，不要每节点查。

---

## 6. 缩小用例：二分定位法

程序在某个随机用例上错了，不要盯着 10x10 看。做「delta debugging」：

1. 先把棋盘缩小到 3x3，看还错不错；
2. 去掉障碍/棋子，一次只留一个元素；
3. 把 `k1/k2` 固定成 1，看是否还错；
4. 在 `genMoves` 里打印走法，和 oracle `--moves` 逐行 diff；
5. 在 `dfs` 里打印 `(depth, key, value)`，找第一个和预期不符的节点。

> 能用手算的 3x3 复现的 bug，才是好 bug。10x10 上的 bug 大多能缩到 3x3。

---

## 7. 常见错误速查

| 现象 | 最可能的原因 | 第一步 |
|---|---|---|
| 段错误 | 数组越界/空指针/递归爆栈 | ASan + gdb bt |
| 死循环 | 循环检测没生效/走法生成不前进 | 打印节点数，Ctrl-C 看栈 |
| 小样例对、随机错 | 边界（列越界/己方棋子） | 缩小用例 + 对拍 |
| 结果随机变 | 未初始化/哈希用了随机种子 | `-fsanitize=memory`（clang）或初始化检查 |
| 越优化越慢 | TT 太大/哈希冲突/每次 clear | perf + 统计节点数 |
| 内存爆 | TT 无上限/状态存了 vector | 给 TT 设上限，用固定数组 |

---

## 8. 一周工具练习（W1 内完成）

- [ ] 用 gdb 在一个故意越界的程序上打出 `bt` 和 `p`。
- [ ] 用 ASan 抓一个 `vector` 越界。
- [ ] 用 `perf stat` 对比 `-O0` 和 `-O2` 的 `instructions`。
- [ ] 用 `steady_clock` 测 `genMoves` 10 万次的平均耗时。
- [ ] 用二分法把一个 5x5 的 WA 缩到 3x3。
