# 05 八股地图 + 手撕清单

> 用法：不要从头背。先看每节的「如果只记三句话」，再按优先级补。
> 每题给自己 **60 秒**口述，卡住就标记，晚上查。W5 起每周 30 分钟；W9–W12 主用。

优先级：**A/B/D > C > H > E/F/G**。
理由：九坤一面/二面大量问线程、内存、缓存、C++ 对象模型；RDMA/bypass 是加分项；docker/k8s 只在笔试选择题。

---

## 0. 你提到的四个词，分别对应什么

你之前说「bypass、线程、kernel 面向对象、互锁」基本不懂。先把词对上：

| 你听到的词 | 更准确的说法 | 本文件位置 | 一句话 |
|---|---|---|---|
| **bypass** | kernel bypass / 内核旁路 | C5–C7 | 绕过内核协议栈，用户态直接收发包（DPDK）或网卡直接读写内存（RDMA） |
| **线程** | thread / 同步 / 内存序 | A1–A9 | 共享地址空间，靠锁/原子/条件变量协作 |
| **kernel 面向对象** | 很可能是「**内核对象**」（Windows HANDLE）或「内核态 C++」 | A12 / F | Windows 里内核对象有句柄、引用计数、可等待状态；Linux 对应「一切皆文件」+ fd |
| **互锁** | **Interlocked** 原子操作 / 自旋锁 / mutex | A5–A7 | Windows 的 `InterlockedIncrement` 是全屏障原子 RMW；Linux 用 `std::atomic`/futex |

> 如果面试官做的是 **Windows 低延迟交易**：重点补 `HANDLE`、`WaitForSingleObject`、`IOCP`、`InterlockedXxx`、`CRITICAL_SECTION`、SEH。
> 如果是 **Linux**：重点补 `fd`、`epoll`、`pthread`、`futex`、`std::atomic`、`io_uring`。
> 两边共同的底层：cache line、原子指令、内存序、系统调用成本。


---

## 0.5 选择题 vs 手撕：计组、图论、运维各占多少

先纠正三个常见误解（基于公开面经，不是官方考纲）：

- **计组：确实考**，而且是选择题的主力之一。例：栈的弹出顺序、堆的性质、进程堆/栈大小、快排最坏复杂度、虚继承/对象布局、cache/字节序。
- **「运维」：不是传统 SRE**。面经里出现的是 docker/k8s、1 号进程、namespace/cgroup、socket、RDMA——属于「容器/云原生 + 网络基础设施」，不考 shell 脚本、监控告警、部署流水线。
- **图论：没有被点名**。选择题最多考「图的存储/遍历/最短路复杂度」这种概念；不会让你在选择题里手推 Dijkstra。图论主要服务于**手撕/大题**（F8），不是选择题重点。

| 板块 | 选择题（25+12） | 手撕/大题 | 复习位置 | 建议投入 |
|---|---|---|---|---|
| 计组 | **高**：堆栈/堆/cache/字节序/复杂度 | 低 | 本文件 E | 8h + Anki |
| OS/线程 | **高**：进程线程/锁/内存 | 中：线程池/共享内存 | A | 10h + Anki |
| 网络/RDMA | **高**：TCP/socket/epoll/RDMA | 低 | C | 6h + Anki |
| 容器/「运维」 | **中高**：docker/k8s/1 号进程 | 低 | F | 4h + Anki |
| C++ | **高**：继承/虚函数/对象布局 | **高**：LRUCache/日期类 | D + H | 12h |
| 数据结构 | **高**：栈/堆/树/哈希/复杂度 | 中 | 本文件 + F1/F2 | 随题练 |
| 图论 | **低–中**：概念/复杂度 | 中：BFS/Dijkstra/DSU | F8 | 按 02 的 5 题 |
| 概率/排列组合 | 中：全错位/期望 | 低 | G | 3h + Anki |

**结论**：

1. 选择题的复习重心是 **计组 + OS + 网络 + 容器 + C++**，不是图论。
2. 图论按 `02_plan90.md` 的 F8 练手撕即可，不要为选择题去刷 1900 的图论题。
3. 「运维」只需要能讲清：容器怎么隔离、1 号进程、k8s 组件、socket 收发、RDMA 为什么快。**不学 K8s 实操**。

> 面经是个例。如果你拿到的笔试通知/真题里明确有图论，再按真题调整；不要用一份回忆录当考纲。

---

## A. 进程、线程、同步（12 题）

### 如果只记三句话

1. 线程共享地址空间/堆/全局变量/fd，独享栈/寄存器/TLS。
2. 锁解决互斥，原子 + 内存序解决可见性与重排；临界区短用自旋，长用 mutex。
3. 伪共享 = 两个核写同一 cache line 的不同变量，靠 padding 到 64B 解决。

### A1. 进程和线程的区别？

- 进程有独立虚拟地址空间、页表、fd 表；线程共享进程的地址空间、堆、全局/静态区、fd。
- 线程独享栈、寄存器上下文、TLS（`errno`）、信号掩码。
- 切换：同进程线程切换不换页表，TLB 通常不失效；进程切换要换页表（PCID 可减少 TLB 刷新）。
- 创建/通信成本：进程 > 线程；隔离性：进程 > 线程。

### A2. 协程是什么？和线程的区别？

- 用户态调度，保存寄存器/栈，切换不陷内核；适合高并发 IO。
- 有栈协程（如 Go）vs 无栈协程（C++20 coroutine，状态机）。
- 不解决 CPU 并行，只解决「等待」的利用率。

### A3. 线程之间共享什么？不共享什么？

共享：代码段、堆、全局/静态变量、打开的文件描述符、当前工作目录、信号处理函数。
不共享：栈、寄存器、TLS、`errno`、信号掩码、`setjmp` 上下文。

### A4. 进程间通信有哪些？

管道/FIFO、消息队列、共享内存、信号量、socket、信号、文件映射。
共享内存最快（零拷贝），但要自己做同步；socket 最通用。

### A5. mutex 和 spinlock 怎么选？

- 临界区极短、多核、不可睡眠上下文 → 自旋。
- 临界区长、可能阻塞、单核 → mutex（睡眠，让出 CPU）。
- 自旋浪费 CPU；mutex 有两次上下文切换成本。

### A6. 死锁的四个必要条件？怎么预防？

互斥、占有且等待、不可抢占、循环等待。
预防：固定加锁顺序、超时/`try_lock`、一次性申请、锁粒度拆分；检测：资源分配图。

### A7. `std::atomic` 和内存序？

- `relaxed`：只保证原子性，不保证顺序。
- `acquire/release`：建立 happens-before（release 写 → acquire 读）。
- `seq_cst`：全局顺序，最贵。
- x86 是 TSO，store-store 不重排，load-load 不重排；但 store-load 可重排，所以需要屏障。

### A8. 什么是伪共享？怎么解决？

两个线程分别写同一 cache line 里的不同变量， cache line 在核间反复失效（false sharing）。
解决：`alignas(64)` 填充、按线程分片、只读共享。

### A9. 线程池的核心组成？

任务队列 + 固定 worker + mutex/condition_variable + 关闭标志；提交返回 `future`（`packaged_task`）。
注意：异常捕获、队列上限、优雅退出、负载均衡。

### A10. `fork()` 后发生了什么？循环 fork 有几个进程？

- 写时复制（COW）：页表复制，物理页共享，写时复制。
- 子进程继承 fd、内存、信号处理；不继承父进程的线程。
- `for (i=0;i<n;i++) fork();` → `2^n` 个进程（含父）。

### A11. 用户态和内核态怎么切换？

系统调用、中断、异常。切换要保存/恢复上下文、换栈、可能刷新 TLB；成本约几百纳秒到微秒。
`vDSO` 让部分系统调用（如 `gettimeofday`）不陷内核。

### A12. 容器里的 1 号进程是什么？

PID 1，负责回收孤儿进程、转发信号。应用不是 init 时，`kill 1` 容器直接退出；用 `tini`/`--init`。
容器退出 ≠ 宿主机内核退出；隔离靠 namespace + cgroup。

---

## B. 内存、缓存、性能（10 题）

### 如果只记三句话

1. 虚拟内存给你隔离、共享、超额分配；代价是页表/TLB。
2. 性能先看 cache：局部性、cache line、预取、AoS/SoA。
3. 分配器是性能热点：少 malloc、批量、对象池、对齐。

### B1. 虚拟内存的作用？

隔离（每个进程独立地址空间）、共享（共享库/共享内存）、超额分配（换页）、权限控制（只读/可执行）。
实现：多级页表 + TLB + 缺页异常。

### B2. `malloc` 怎么工作？

小块走 `brk`/arena + bin（tcache/fastbin/smallbin），大块 `mmap`；
free 不立即还 OS，进空闲链表；碎片来自分配模式。
`tcmalloc/jemalloc` 用线程缓存 + size class 降低竞争。

### B3. cache 层次和 cache line？

L1 32–64KB（指令/数据分离）、L2 256KB–1MB、L3 共享；line 通常 64B。
命中率取决于时间/空间局部性；顺序访问 > 随机访问；步长 > line 会浪费带宽。

### B4. 怎么分析缓存命中？

`perf stat -e cache-misses,cache-references`；`perf record` + 火焰图；
看 working set 是否超过 L2/L3；看步长、分支、预取。

### B5. 伪共享和 cache line 对齐？

见 A8。补充：`alignas(64)`、`std::hardware_destructive_interference_size`（C++17）。

### B6. NUMA 是什么？

多路 CPU，每颗有本地内存；跨 node 访问延迟高。
解决：绑核（`numactl --cpunodebind`）、本地分配（`numa_alloc_onnode`）、线程/内存同 node。

### B7. 大页（huge page）有什么用？

2MB/1GB 页，减少页表层级和 TLB miss；DPDK/数据库/虚拟化常用。
代价：分配/回收粒度大、可能浪费内存。

### B8. 零拷贝有哪些手段？

`sendfile`（内核态直接发）、`mmap` + `write`、`splice`、RDMA。
减少用户/内核态拷贝和上下文切换；不是「完全不拷贝」。

### B9. 内存屏障是什么？

阻止编译器/CPU 重排；`std::atomic_thread_fence`、`asm volatile("" ::: "memory")`。
x86 的 `mfence/lfence/sfence`；acquire/release 通常编译成较轻的指令。

### B10. 性能优化的顺序？

先测量（profiler）→ 找热点 → 算法/数据结构 → 内存布局/缓存 → 减少分配/拷贝 → 并行 → 底层指令。
不要先写 SIMD。

---

## C. 网络、RDMA、kernel bypass（8 题）

### 如果只记三句话

1. TCP 是字节流：`send` 只保证拷进内核缓冲区，不保证对端收到；要自己 framing。
2. epoll 用红黑树 + 就绪链表；ET 必须非阻塞 + 读到 `EAGAIN`。
3. RDMA/DPDK 都是 kernel bypass：网卡直接读写用户态内存，省掉内核协议栈和拷贝。

### C1. TCP 和 UDP 的区别？

TCP：面向连接、可靠、有序、字节流、拥塞控制、流量控制。
UDP：无连接、不可靠、保留边界、开销小；适合低延迟/广播/自建可靠层。

### C2. `send/recv` 的返回值说明什么？

- `send` 返回「拷进内核发送缓冲区」的字节数，不表示对端收到；可能短写。
- `recv` 返回「当前可读」的字节数；0 表示对端关闭；可能短读。
- 应用层要长度前缀/分隔符；不要假设一次收完整包。

### C3. epoll 比 select/poll 强在哪？

select/poll 每次 O(n) 遍历；epoll 用红黑树管理 fd + 就绪链表，只返回就绪的。
LT（水平触发）可阻塞；ET（边缘触发）必须非阻塞并一次读到 `EAGAIN`。

### C4. Reactor 和 Proactor？

Reactor：就绪事件通知，用户自己读写（epoll）。
Proactor：完成事件通知，内核/网卡完成 IO（IOCP、io_uring、RDMA）。

### C5. RDMA 是什么？

Remote Direct Memory Access：网卡直接读写远端**已注册**内存，绕过内核协议栈和 CPU 拷贝。
两种语义：one-sided（read/write，远端 CPU 不参与）、two-sided（send/recv）。
需要 RNIC、内存注册（pin）、无损网络（PFC/ECN）。

### C6. kernel bypass 是什么？DPDK 怎么做？

绕过内核网络栈，用户态直接收发包。
DPDK：PMD 轮询、UIO/VFIO、hugepage、绑核、无锁 ring；代价是占一个核、自己实现协议栈。

### C7. RDMA 的 one-sided write，远端怎么知道？

远端 CPU **不会**被通知（这是 one-sided 的意义）；需要：
- 轮询 completion queue（本端完成）；
- 或额外发一条 send/recv 消息；
- 或用内存写触发事件（部分硬件支持）。
面试点：区分「本端完成」和「远端可见」。

### C8. 延迟敏感系统还关心什么？

绑核/隔离、无锁队列、批量、避免缺页（预分配/大页）、避免系统调用、NUMA、时间戳硬件（TSC/PTP）。

---

## D. C++ 对象模型与现代 C++（14 题）

### 如果只记三句话

1. 虚函数 = vptr + vtable；虚继承 = 虚基类偏移，解决菱形。
2. 移动语义 = 偷资源；`noexcept` 影响 `vector` 扩容用 move 还是 copy。
3. `shared_ptr` = 指针 + 控制块（强/弱计数 + deleter）；循环引用用 `weak_ptr`。

### D1. 虚函数怎么实现？

每个多态类一张 vtable（虚函数地址表），每个对象一个 vptr 指向它。
调用：取 vptr → 取表项 → 间接跳转；难以内联，有分支预测开销。

### D2. 虚继承解决什么？对象布局怎么变？

解决菱形继承（`B`、`C` 继承 `A`，`D` 继承 `B`、`C`，`A` 只保留一份）。
实现：虚基类偏移表（vbtable）/指针，`D` 对象里只有一份 `A`；访问虚基类成员要多一次偏移计算。
`virtual` 继承 ≠ 虚函数。

### D3. 移动语义做了什么？

右值引用 + 移动构造/赋值：偷指针/缓冲区，把源置空；避免深拷贝。
`noexcept` 很重要：`vector` 扩容时，移动构造若可能抛异常，会退回拷贝以保证强异常安全。

### D4. RAII 是什么？

构造获取资源、析构释放；析构在栈展开时自动调用，保证异常安全。
例子：`unique_ptr`、`lock_guard`、`fstream`。

### D5. `shared_ptr` 的实现？

对象指针 + 控制块（强计数、弱计数、deleter、allocator）。
计数用原子操作；`make_shared` 一次分配对象 + 控制块；`weak_ptr` 不增加强计数，用于打破循环。

### D6. `unique_ptr` 为什么零开销？

只存一个指针 + 编译期 deleter；无引用计数；移动语义转移所有权。

### D7. allocator 是干什么的？

把「分配未构造内存」和「构造对象」分离：`allocate(n)` / `deallocate(p,n)` / `construct` / `destroy`。
容器通过 `allocator_traits` 使用；可以接内存池、对齐分配、统计。

### D8. `vector` 扩容为什么是均摊 O(1)？

按倍数（1.5/2）扩容，总复制次数是等比级数；均摊到每次 `push_back` 是 O(1)。
扩容会使迭代器/引用失效；`reserve` 可避免。

### D9. `map` / `unordered_map` 的区别？

`map`：红黑树，O(log n)，有序，稳定；`unordered_map`：哈希，平均 O(1)，最坏 O(n)，无序。
哈希表要关注负载因子、rehash、哈希质量。

### D10. `list` 和 `vector` 怎么选？

`vector`：连续内存、cache 友好、随机访问；中间插入/删除 O(n)。
`list`：O(1) 插入/删除（已知位置），但每个节点单独分配、cache 不友好、无随机访问。

### D11. AoS 和 SoA 的区别？

AoS：`struct {x,y,z;}` 数组，一次访问拿全字段，适合「整体使用」。
SoA：`x[] y[] z[]`，适合只处理某一字段、SIMD、cache 利用率高。
量化/粒子/图形常用 SoA。

### D12. 编译链接过程？

预处理（宏/头文件）→ 编译（IR/汇编）→ 汇编（目标文件）→ 链接（符号解析/重定位）。
静态库在链接时复制；动态库在加载时解析（`-fPIC`、`GOT/PLT`）。

### D13. 什么是 ODR / UB？

ODR：一个实体全程序只能有一个定义（inline/模板例外）。
UB：越界、悬垂指针、有符号溢出、严格别名违规；编译器可假设不发生。

### D14. C++ 内存模型里 object lifetime 是什么？

对象在构造完成时开始、析构开始时结束；在 lifetime 外访问是 UB。
`placement new`、`std::launder`、`memcpy` 的边界都和它相关。

---

## E. 计组（6 题，笔试为主）

1. **MESI**：Modified/Exclusive/Shared/Invalid，缓存一致性协议。
2. **分支预测**：预测错误惩罚 15–20 周期；排序后遍历可减少误预测。
3. **流水线**：取指/译码/执行/访存/写回；冒险（数据/控制/结构）。
4. **乱序/超标量**：指令级并行；寄存器重命名；ROB。
5. **内存墙**：CPU 快、内存慢，靠 cache/预取/局部性。
6. **对齐/字节序**：`struct` padding；小端 x86；网络字节序大端。

---

## F. docker / k8s（6 题，笔试为主）

1. **容器隔离**：namespace（PID/NET/MNT/UTS/IPC/USER）+ cgroup（CPU/内存）+ rootfs。
2. **镜像分层**：只读层 + 可写层（overlayfs）；Dockerfile 每条指令一层；多阶段构建。
3. **1 号进程**：见 A12。
4. **k8s 组件**：apiserver、etcd、scheduler、controller-manager、kubelet、kube-proxy。
5. **Pod/Service/Ingress**：最小调度单元 / 稳定访问入口 / 七层路由。
6. **容器 vs 虚拟机**：共享内核 vs 独立内核；启动快、密度高 vs 隔离强。

---

## G. 量化 / 金融 / 概率（8 题）

1. **期货**：标准化合约、保证金、每日无负债结算、到期交割；杠杆放大盈亏。
2. **期权**：买方付权利金获得权利；call/put；欧式（到期行权）/美式（随时）；Greeks（Delta/Gamma/Vega/Theta）。
3. **最大回撤**：`max_t (peak_t - value_t) / peak_t`；衡量最坏回撤幅度。
4. **自动微分**：前向模式（每步传播导数）vs 反向模式（计算图反向传播，深度学习常用）；区别于数值微分（近似）和符号微分（表达式膨胀）。
5. **GIL**：CPython 全局解释器锁，同一时刻只有一个线程执行字节码；IO 密集可多线程，CPU 密集用多进程/扩展。
6. **全错位排列**：`D(n) = (n-1)(D(n-1)+D(n-2))`，`D(1)=0, D(2)=1`；`D(4)=9`。
7. **3L/5L 量 4L**：装满 5 → 倒入 3（5 剩 2）→ 倒空 3 → 2 倒入 3 → 装满 5 → 往 3 倒到满（只倒 1）→ 5 剩 4。
8. **numpy/pandas**：`ndarray` 连续内存 + 向量化；`DataFrame` 列式/索引对齐；避免逐行循环。

---

## H. 手撕清单（12 题）

| # | 题目 | 时间 | 必答点 | 常见追问 |
|---|---|---|---|---|
| 1 | **LRUCache** | 25m | `list` + `unordered_map`，get/put O(1) | 容量 0、put 已存在、线程安全 |
| 2 | **日期类** | 30m | 闰年、月天数、两日期相减 | 跨月/跨年、非法日期、时区 |
| 3 | **`vector`** | 40m | RAII、capacity、扩容、异常安全 | 迭代器失效、`reserve`、`shrink_to_fit` |
| 4 | **`shared_ptr`** | 30m | 控制块、引用计数、deleter | 循环引用、线程安全、`make_shared` |
| 5 | **线程池** | 40m | 任务队列、CV、`packaged_task` | 优雅退出、异常、队列满 |
| 6 | **生产者消费者** | 25m | 有界队列、mutex + CV | 虚假唤醒、通知时机 |
| 7 | **单例** | 15m | Meyers 单例、线程安全 | 析构顺序、禁止拷贝 |
| 8 | **内存池** | 40m | free list、对齐、块大小 | 碎片、线程安全 |
| 9 | **无锁队列** | 45m | CAS、head/tail | ABA、内存序 |
| 10 | **计时装饰器（Python）** | 15m | `functools.wraps`、`time.perf_counter` | 带参数、异步 |
| 11 | **二分/前缀和/排序** | 20m | 边界、`lower_bound` | 重复元素、溢出 |
| 12 | **并查集/Dijkstra** | 30m | 路径压缩、优先队列 | 负权、复杂度 |

### LRUCache 的最小实现骨架

```cpp
class LRUCache {
    int cap;
    list<pair<int,int>> L;                       // front = 最近使用
    unordered_map<int, list<pair<int,int>>::iterator> M;
public:
    LRUCache(int c): cap(c) {}
    int get(int k) {
        auto it = M.find(k);
        if (it == M.end()) return -1;
        L.splice(L.begin(), L, it->second);      // O(1) 移到前面
        return it->second->second;
    }
    void put(int k, int v) {
        if (cap == 0) return;
        if (auto it = M.find(k); it != M.end()) {
            it->second->second = v;
            L.splice(L.begin(), L, it->second);
            return;
        }
        if ((int)L.size() == cap) {
            M.erase(L.back().first);
            L.pop_back();
        }
        L.emplace_front(k, v);
        M[k] = L.begin();
    }
};
```

### 日期类的最小骨架

```cpp
struct Date {
    int y, m, d;
    static bool leap(int y) { return (y%4==0 && y%100!=0) || y%400==0; }
    static int dim(int y, int m) {
        static int a[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        return (m==2 && leap(y)) ? 29 : a[m];
    }
    int toDays() const {  // 1970-01-01 起
        int days = 0;
        for (int i = 1970; i < y; ++i) days += leap(i) ? 366 : 365;
        for (int i = 1; i < m; ++i) days += dim(y, i);
        return days + d - 1;
    }
    int operator-(const Date& o) const { return toDays() - o.toDays(); }
};
```

> 追问「怎么更快」：用儒略日公式/查表；追问「时区/闰秒」：承认边界，说明取舍。

---

## I. 如果只有 3 天

| 天 | 上午 | 下午 | 晚上 |
|---|---|---|---|
| D1 | A1–A8 + B1–B5 | 手撕 LRUCache + 日期类 | Anki A/B |
| D2 | C1–C7 + D1–D7 | 手撕 `vector` + `shared_ptr` | Anki C/D |
| D3 | E/F/G 速记 + 手撕线程池 | 模拟面试 1 次（60 分钟） | 错题 + 录音 |

3 天不可能全背完。策略：**A/B/D 必须能讲，C 能讲 3 句，E/F/G 混脸熟**。

---

## J. 一周速记表（贴在墙上）

| 主题 | 一句话 |
|---|---|
| 进程/线程 | 独立地址空间 vs 共享堆 |
| 协程 | 用户态调度，省内核切换 |
| 伪共享 | 同 cache line 不同变量，padding |
| 内存序 | acquire/release 建 happens-before |
| 虚拟内存 | 页表 + TLB + 缺页 |
| RDMA | 网卡直读注册内存，绕过内核 |
| DPDK | 用户态轮询 + hugepage + 绑核 |
| 虚函数 | vptr → vtable → 间接跳转 |
| 虚继承 | 虚基类只留一份，偏移表 |
| 移动语义 | 偷资源，`noexcept` 关键 |
| shared_ptr | 控制块 + 强弱计数 |
| allocator | 分配/构造分离，可接内存池 |
| AoS/SoA | 整体访问 vs 按字段/SIMD |
| epoll | 红黑树 + 就绪链表，ET 读到 EAGAIN |
| 容器 | namespace + cgroup + 分层镜像 |
