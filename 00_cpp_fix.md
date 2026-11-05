
# 00 C++ 急救包：3 天，把 4.5/10 补到 7–8/10

> 先说结论：**4.5/10 不是「你基础很差」，是「你缺几组具体语义」**。
> 你能写构造/析构、能写阶乘、知道指针和引用的大致区别——说明代码手感和递归没问题。
> 但你的 C++ 是「CF 风格」：能用 `vector`/`sort`，但说不清它们的语义；一遇到
> 拷贝/移动、`emplace_back`/`reserve`、lowbit、虚析构，就开始猜。
> 这不是智商问题，是**没人给你系统讲过一遍**。3 天可以补到「答对 7–8 题」。

---

## 0. 你的实际批改

| #  | 题                     | 你的答案                                                | 判定 | 一句话纠正                                                  |
| -- | ---------------------- | ------------------------------------------------------- | ---- | ----------------------------------------------------------- |
| 1  | 构造/析构/拷贝/移动    | 构造析构会；拷贝移动不熟                                | ⚠️ | 拷贝=深拷贝，移动=偷指针；见 `IntVec`                     |
| 2  | `const T&` vs `T&` | 不能修改时用 const                                      | ⚠️ | 还差「不拷贝、可绑临时对象」                                |
| 3  | vector 三件套          | `emplace_back` 猜成拼接；`reserve` 说成 `reverse` | ❌   | 见 §3                                                      |
| 4  | map 统计               | 会写，但口述成 `vector<map<string,int>>`              | ✅   | 一个 `map<string,int>` 就够；多个文档才用 `vector<map>` |
| 5  | 阶乘/记忆化            | 阶乘会；两个变量                                        | ⚠️ | 两个变量是迭代，不是记忆化                                  |
| 6  | lowbit/popcount        | `x&-x` 说 false；popcount 不认识                      | ❌   | lowbit；数 1 的个数                                         |
| 7  | 第二关键字排序         | `greater<int>`                                        | ❌   | 要写比较器 lambda                                           |
| 8  | 指针/引用、堆/栈       | 指针/引用大致对                                         | ⚠️ | 还差 `new/delete` vs 栈对象                               |
| 9  | undefined reference    | 没定义变量                                              | ⚠️ | 链接错误：声明有、定义没有/没链库                           |
| 10 | 虚函数/虚析构          | 子类覆盖；虚析构让子类覆盖                              | ⚠️ | 动态绑定；虚析构保证删基类指针时析构派生                    |

**你真正缺的是 4 组**：对象生命周期（1/2/8/10）、STL 容器语义（3/7）、位运算（6）、记忆化/链接（5/9）。
map 算你会，不占缺口。不是 10 个孤立的点。

---

## 1. 三天安排（每天 2–2.5h，和 W1 冷解并行）

| 天           | 主题                        | 练习（必须手写）                                                       | 验收                                              |
| ------------ | --------------------------- | ---------------------------------------------------------------------- | ------------------------------------------------- |
| **D1** | 类生命周期、引用、拷贝/移动 | `IntVec`（动态数组，Rule of Five）                                   | 能解释为什么要虚析构、什么是对象切片              |
| **D2** | STL、RAII、智能指针         | 词频（快速过）+ 第二关键字排序 + `unique_ptr` + lowbit/popcount/链接 | 能说清 `push_back`/`emplace_back`/`reserve` |
| **D3** | 继承、多态、虚函数          | `Shape`/`Circle`/`Rect` + `vector<unique_ptr<Shape>>`          | 重做 10 题，≥7                                   |

> 不要停掉 W1。每天：2h 急救包 + 1 道 W1 的题（LC 289 / CF 793B）。
> 急救包是「补术语」，题目是「保持输出」。

---

## 2. D1：类生命周期

### 必须能背出来的规则

- 析构函数：对象生命周期结束时自动调用；基类析构要 `virtual`，否则 `delete base_ptr` 是 UB。
- 拷贝构造：`T(const T&)`；移动构造：`T(T&&) noexcept`；拷贝赋值/移动赋值。
- Rule of Five：析构、拷贝构造、拷贝赋值、移动构造、移动赋值，写一个就要想另外四个。
- `= default` / `= delete`；`explicit` 防止隐式转换。
- `const T&`：只读、不拷贝、能绑定临时对象；`T&`：可修改、不能绑定临时对象。

### 练习：`IntVec`

```cpp
#include <algorithm>
#include <cstddef>
#include <utility>

class IntVec {
public:
    IntVec() = default;
    explicit IntVec(std::size_t n) : p_(new int[n]{}), n_(n), cap_(n) {}
    ~IntVec() { delete[] p_; }

    IntVec(const IntVec& o) : p_(new int[o.n_]), n_(o.n_), cap_(o.n_) {
        std::copy(o.p_, o.p_ + n_, p_);
    }
    IntVec(IntVec&& o) noexcept : p_(o.p_), n_(o.n_), cap_(o.cap_) {
        o.p_ = nullptr; o.n_ = o.cap_ = 0;
    }
    // copy-swap：同时处理拷贝赋值和移动赋值
    IntVec& operator=(IntVec o) {
        swap(o);
        return *this;
    }
    void swap(IntVec& o) noexcept {
        std::swap(p_, o.p_);
        std::swap(n_, o.n_);
        std::swap(cap_, o.cap_);
    }

    std::size_t size() const { return n_; }
    int& operator[](std::size_t i) { return p_[i]; }
    const int& operator[](std::size_t i) const { return p_[i]; }

private:
    int* p_ = nullptr;
    std::size_t n_ = 0, cap_ = 0;
};
```

**自己加**：`push_back`（扩容 2 倍）、`reserve`、`clear`。写完后用 ASan 跑一遍。

### 必答

- 为什么 `IntVec(IntVec&&) noexcept` 的 `noexcept` 重要？（`vector` 扩容时，移动可能抛异常就退回拷贝）
- 什么是对象切片？（派生类按值赋给基类，只保留基类部分）
- 为什么基类析构要 `virtual`？

---

## 3. D2：STL、RAII、智能指针

### 必须能背出来的规则

- `push_back`：拷贝/移动一个已有对象；`emplace_back`：在容器内原地构造；`reserve`：只改 capacity，不改 size。
- `vector` 扩容按倍数，均摊 O(1)；扩容后迭代器/指针/引用失效。
- `map` 红黑树 O(log n)，有序；`unordered_map` 哈希平均 O(1)，无序。
- `unique_ptr`：独占、零开销、移动转移所有权；`shared_ptr`：控制块 + 强弱计数；循环引用用 `weak_ptr`。
- RAII：构造获取资源，析构释放；异常安全靠栈展开。

### 练习

```cpp
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

std::map<std::string, int> word_count(const std::vector<std::string>& ws) {
    std::map<std::string, int> m;
    for (const auto& w : ws) ++m[w];
    return m;
}

struct Node { int x, y; };

void sort_by_y(std::vector<Node>& v) {
    std::sort(v.begin(), v.end(), [](const Node& a, const Node& b) {
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    });
}

void demo() {
    auto p = std::make_unique<int>(42);
    std::vector<std::unique_ptr<int>> v;
    v.push_back(std::move(p));   // unique_ptr 不能拷贝，只能移动
}
```

### 把这三个词彻底分清

```cpp
std::vector<int> a{1, 2}, b{3, 4};

a.push_back(5);                              // 把 5 拷贝/移动进尾部
a.emplace_back(6);                           // 在尾部原地构造 6（参数直接传给构造函数）
a.insert(a.end(), b.begin(), b.end());       // 把 b 拼到 a 后面（不是 emplace_back）
a.reserve(100);                              // capacity >= 100，size 不变
std::reverse(a.begin(), a.end());            // 反转；reserve 不是 reverse
```

- `emplace_back(args...)` 一次只放**一个**元素，只是把 `args` 直接传给构造函数，省一次临时对象。
- 拼接用 `insert`，不是 `emplace_back`。
- `reserve` 是「预留容量」，`reverse` 是「反转」，两个词不要混。

### 比较器的三种写法（`sort` 都能接）

```cpp
struct Node { int x, y; };

// 1) 普通函数
bool cmp_fn(const Node& a, const Node& b) {
    if (a.y != b.y) return a.y < b.y;   // 先按 y 升序
    return a.x < b.x;                   // y 相同按 x 升序
}
sort(v.begin(), v.end(), cmp_fn);

// 2) lambda（最常用）
sort(v.begin(), v.end(), [](const Node& a, const Node& b) {
    if (a.y != b.y) return a.y < b.y;
    return a.x < b.x;
});

// 3) 函数对象（priority_queue 需要这种）
struct Cmp {
    bool operator()(const Node& a, const Node& b) const {
        return a.y > b.y;               // 按 y 降序
    }
};
sort(v.begin(), v.end(), Cmp{});
```

`greater<int>()` 只是第 3 种的一个现成版本，等价于：

```cpp
struct GreaterInt {
    bool operator()(int a, int b) const { return a > b; }
};
sort(v.begin(), v.end(), GreaterInt{});
```

> `greater<int>` 不是“比较器工厂”：不能把自定义比较方法塞进 `greater` 里；它本身就是 `a > b` 的比较器。要自定义，就自己写函数 / lambda / 函数对象。

**比较器必须满足严格弱序**：相等时返回 `false`，所以写 `<` 或 `>`，不要写 `<=` 或 `>=`；否则 `sort` 可能越界/死循环。

### 补漏：lowbit、popcount、undefined reference

```cpp
int lowbit(int x) { return x & -x; }                  // 12(1100) -> 4(0100)
int pc(unsigned long long x) { return __builtin_popcountll(x); } // 12 -> 2
```

- `x & -x` 取**最低位的 1**，不是 `false`；这是树状数组/状压的常用操作。
- `x & (x-1)` 清除最低位的 1。
- `__builtin_popcountll` 数 64 位里 1 的个数。

**`undefined reference to ...` 是链接错误，不是语法错误**：

- 编译阶段只检查声明；链接阶段才找定义。
- 常见原因：函数/变量只有声明没有定义；`a.cpp` 写了但没参与编译；忘了链接库；签名/命名空间不一致。
- 例：`// a.h: void f();`，`main.cpp` 调用 `f()`，但编译时只写了 `g++ main.cpp`，没带 `a.cpp` → `undefined reference to f()`。

### 必答

- `push_back` 和 `emplace_back` 的区别？什么时候 `emplace_back` 更快？
- `reserve(100)` 之后 `size()` 是多少？`capacity()` 呢？
- `unique_ptr` 为什么不能拷贝？`shared_ptr` 的计数在哪个结构里？
- `new/delete` 和栈对象的区别？什么时候必须用堆？

---

## 4. D3：继承、多态、虚函数

### 必须能背出来的规则

- 虚函数：vtable + vptr，运行时动态绑定；调用是一次间接跳转，通常不能内联。
- 虚析构：通过基类指针删除派生类对象时，必须 `virtual ~Base()`。
- `override`：让编译器检查签名；`final`：禁止继续重写。
- 纯虚函数 `= 0`：抽象类，不能实例化。
- 虚继承：解决菱形继承，虚基类只保留一份（对象布局更复杂，知道概念即可）。

### 练习：Shape

```cpp
#include <cmath>
#include <memory>
#include <vector>

struct Shape {
    virtual ~Shape() = default;
    virtual double area() const = 0;
};

struct Circle : Shape {
    explicit Circle(double radius) : r(radius) {}
    double area() const override { return std::acos(-1.0) * r * r; }
    double r;
};

struct Rect : Shape {
    Rect(double width, double height) : w(width), h(height) {}
    double area() const override { return w * h; }
    double w, h;
};

double total(const std::vector<std::unique_ptr<Shape>>& v) {
    double s = 0;
    for (const auto& p : v) s += p->area();
    return s;
}
```

### 必答

- `virtual` 函数怎么实现？对象里多了什么？
- 为什么基类析构要虚？不虚会怎样？
- 虚继承和虚函数是一回事吗？
- 什么是对象切片？怎么避免？

---

## 4.5 模板：只学面试会问的那点（30 分钟）

> STL 全是模板，不学模板等于看不懂 `vector<int>` 为什么是个类型。
> 但 **不要**碰模板元编程、SFINAE、concepts、可变参数模板。

### 三段代码

```cpp
// 1. 函数模板
template <typename T>
T mymax(T a, T b) { return a < b ? b : a; }

// 2. 类模板
template <typename T>
class Stack {
    std::vector<T> v;
public:
    void push(const T& x) { v.push_back(x); }
    void pop()            { v.pop_back(); }
    const T& top() const  { return v.back(); }
    bool empty() const    { return v.empty(); }
};

// 3. 非类型模板参数（std::array 就是这么做的）
template <typename T, int N>
struct Arr { T a[N]; };
Arr<int, 8> buf;
```

### 四条结论（面试就考这些）

1. 模板是编译期的「印章」：`Stack<int>` 和 `Stack<double>` 是**两个不同的类型**，各生成一份代码。
2. 模板定义必须对使用点可见，所以**放头文件**；拆成 `.h/.cpp` 会在链接期报 `undefined reference`。
3. 模板 = 编译期多态（静态）；虚函数 = 运行期多态（动态）。成员函数模板**不能是 `virtual`**。
4. 特化/偏特化知道概念就行（`template <> struct hash<MyType>`），不写。

### learncpp 对应

- 函数模板：**11.6**、**11.7**（实例化）
- 类模板：**13.13**（13.14 CTAD 扫一眼）
- 26.x（类模板特化）跳过

---

## 5. 10 题答案速查（先自己做，再对）

1. **Rule of Five**：见 `IntVec`；析构/拷贝/移动/赋值，写一个想五个。
2. **`const T&` vs `T&`**：`const T&` 只读、不拷贝、可绑临时对象；`T&` 可写、不能绑临时对象。
3. **`push_back`/`emplace_back`/`reserve`**：`push_back` 拷贝/移动一个已有对象；`emplace_back(args...)` 原地构造一个；拼接用 `insert`；`reserve` 只改 capacity，`reverse` 才是反转。
4. **`map` 统计**：一个 `map<string,int> m; for (auto& w : ws) ++m[w];`，不是 `vector<map<...>>`。
5. **递归/记忆化**：`f(n)=n*f(n-1)`；记忆化斐波那契用 `vector<long long> memo(n+1,-1)`，先查 memo 再递归。两个变量是迭代 DP，不是记忆化。
6. **`x & -x`**：最低位的 1（12 → 4）；`x & (x-1)` 清除最低位；`__builtin_popcountll(x)` 数 1 的个数（12 → 2）。
7. **按第二关键字排序**：`sort(v.begin(), v.end(), [](const Node&a,const Node&b){ return a.y!=b.y ? a.y<b.y : a.x<b.x; });`；`greater<int>` 只能按单关键字降序。
8. **指针 vs 引用**：指针可为空/可重绑/可运算；引用是别名、必须初始化；`new/delete` 手动堆，栈对象自动析构。
9. **`undefined reference`**：链接错误——有声明没定义、源文件没参与编译、没链接库、签名不一致。
10. **虚函数/虚析构**：虚函数靠 vtable/vptr 动态绑定；虚析构不是「让子类覆盖析构」，而是保证 `delete base_ptr` 时先调用派生析构，避免 UB/泄漏。

---

## 6. 如果只有 1 天

按这个顺序，每块 40 分钟：

1. `IntVec` 的构造/析构/拷贝/移动（D1）。
2. `vector` 的 `push_back`/`emplace_back`/`reserve` + `map` 计数（D2）。
3. 虚函数 + 虚析构 + `Shape`（D3）。
4. 重做 10 题，只求能口述，不要求全对。

---

## 7. 和 W1 怎么并行

| 天 | 急救包                           | W1 题目                |
| -- | -------------------------------- | ---------------------- |
| D1 | `IntVec`                       | LC 289（75 分钟冷解）  |
| D2 | 单词计数 + 排序 + `unique_ptr` | CF 793B（75 分钟冷解） |
| D3 | `Shape` + 重测 10 题           | 复盘 + 48h 重写        |

> 不要等「补完 C++」再开始做题。做题会暴露你真正缺的那 1–2 个点，比看视频快。

---

## 8. 三天的验收

- [ ] 能空手写出 `IntVec` 的五个函数，并解释每个为什么存在。
- [ ] 能说清 `push_back` vs `emplace_back` vs `reserve`。
- [ ] 能说清 `unique_ptr` 为什么不能拷贝。
- [ ] 能说清虚函数/虚析构/vtable，以及对象切片。
- [ ] 10 题重测 ≥7。
- [ ] 两道 W1 冷解都跑过（哪怕没 AC，也要有复盘记录）。

做不到第 5 条：把 D1/D3 再各加半天；不要拖过 5 天。
