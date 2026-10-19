// 技能黑白棋 —— 训练骨架（M0/M1 起点）
//
// 编译： g++ -O2 -std=c++17 -Wall -Wextra stub.cpp -o stub
// 运行： ./stub < case.txt        （格式见 code/spec.md）
//        ./stub --moves < case.txt
//
// 你只需要实现三个函数：genMoves / negamax / (可选) zobrist。
// 其余代码是脚手架，建议先读懂再改。
//
// 规则默认值（与 oracle.py 保持一致）：
//   白先手；方向顺序 下/左/右/上；黑走前 k1 个，白走前 k2 个；
//   踩到对方棋子 -> 走子方立即胜；黑走到 goal_row -> 黑立即胜；
//   无合法走法 -> 当前玩家负。
//
// 状态表示：n*m <= 100，用两个 u64 存一方的棋子位图（低位在前，行优先）。
//   idx = r*m + c, bit = 1ULL << (idx % 64), word = idx / 64

#include <bits/stdc++.h>
using namespace std;
using u64 = unsigned long long;

static const int DR[4] = {1, 0, 0, -1};  // 下 左 右 上
static const int DC[4] = {0, -1, 1, 0};

struct Mask {
    u64 v[2] = {0, 0};
    bool test(int i) const { return (v[i >> 6] >> (i & 63)) & 1ULL; }
    void set(int i) { v[i >> 6] |= 1ULL << (i & 63); }
    void reset(int i) { v[i >> 6] &= ~(1ULL << (i & 63)); }
    bool empty() const { return !v[0] && !v[1]; }
};

struct State {
    Mask b, w;
    int turn;  // 0 = 白，1 = 黑
};

struct Move {
    int from, to;
    bool win;  // 这一步直接获胜（吃子 / 黑棋突防）
};

struct Game {
    int n, m, k1, k2, goal_row;
    Mask obs;
    State start;

    bool inside(int r, int c) const { return 0 <= r && r < n && 0 <= c && c < m; }
};

// ---------------------------------------------------------------------------
// TODO(M1): 生成当前玩家的所有合法走法。
// 要求：
//   1. 只动自己的棋子；每个棋子只走前 k 个方向。
//   2. 越界 / 障碍 / 己方棋子 -> 非法。
//   3. 目标是对方棋子 -> win = true（立即获胜，不需要展开）。
//   4. 黑棋走到 goal_row -> win = true。
//   5. 返回的 win 走法保留在列表里（方便单测），但搜索时看到 win 可直接返回。
// 提示：遍历 own 的每个 1 bit，用 __builtin_ctzll 取下标。
// ---------------------------------------------------------------------------
vector<Move> genMoves(const Game& g, const State& s) {
    (void)g;
    (void)s;
    vector<Move> moves;
    // ===== 你的代码 =====

    // ===== 你的代码 =====
    return moves;
}

// 执行一步（非 win 走法），返回新状态。
State apply(const State& s, const Move& mv) {
    State t = s;
    if (s.turn == 1) {
        t.b.reset(mv.from);
        t.b.set(mv.to);
    } else {
        t.w.reset(mv.from);
        t.w.set(mv.to);
    }
    t.turn ^= 1;
    return t;
}

// ---------------------------------------------------------------------------
// TODO(M2-M4): negamax + alpha-beta。
// 约定：返回值是“当前走子方”的视角：+1 胜，0 平，-1 负。
//   - moves 为空：返回 -1（无路可走判负）。
//   - 存在 win 走法：返回 +1（与 depth 无关！）。
//   - depth == 0：返回 0（把未知当平局；M3 之后换成置换表 + 迭代加深）。
// 先写不带置换表的版本，跑通对拍，再加 TT。
// ---------------------------------------------------------------------------
int negamax(const Game& g, const State& s, int depth, int alpha, int beta) {
    (void)g;
    (void)s;
    (void)depth;
    (void)alpha;
    (void)beta;
    // ===== 你的代码 =====

    // ===== 你的代码 =====
    return 0;
}

// ---------------------------------------------------------------------------
// 读入 / 输出（不用改）
// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool printMoves = false;
    for (int i = 1; i < argc; ++i)
        if (string(argv[i]) == "--moves") printMoves = true;

    Game g{};
    cin >> g.n >> g.m >> g.k1 >> g.k2;
    g.goal_row = g.n - 1;  // 默认：第 1 行 = 最下面一行

    for (int r = 0; r < g.n; ++r) {
        string row;
        cin >> row;
        for (int c = 0; c < g.m; ++c) {
            int idx = r * g.m + c;
            char ch = row[c];
            if (ch == '#') g.obs.set(idx);
            else if (ch == 'b') g.start.b.set(idx);
            else if (ch == 'w') g.start.w.set(idx);
        }
    }
    g.start.turn = 0;  // 白先

    if (printMoves) {
        cout << "# from_r from_c to_r to_c win\n";
        auto moves = genMoves(g, g.start);
        if (moves.size() > 1)
        sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            if (a.from != b.from) return a.from < b.from;
            return a.to < b.to;
        });
        for (auto& mv : moves) {
            cout << mv.from / g.m << ' ' << mv.from % g.m << ' '
                 << mv.to / g.m << ' ' << mv.to % g.m << ' ' << (mv.win ? 1 : 0) << '\n';
        }
        return 0;
    }

    // TODO(M2): 把下面这行换成 negamax 调用，例如：
    //   int v = negamax(g, g.start, /*depth=*/100, -2, 2);
    //   cout << (v > 0 ? "WHITE" : v < 0 ? "BLACK" : "DRAW") << "\n";
    // 注意：v 是「当前走子方」视角，白先，所以 v>0 是白胜。
    cerr << "[stub] genMoves/negamax 尚未实现，输出 DRAW 占位\n";
    cout << "DRAW\n";
    return 0;
}
