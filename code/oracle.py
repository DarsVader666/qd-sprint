#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
技能黑白棋 —— 精确解算器（小棋盘对拍用）

用途：
  1. 给你的 C++ 程序提供 ground truth（WIN/LOSE/DRAW 是博弈论意义下的精确值）。
  2. 输出初始局面的全部合法走法，用来单测你的 genMoves。
  3. --selftest 用两套独立算法互相验证。

规则（训练版，全部可配置）：
  - n*m 网格，'#' 障碍，'b' 黑，'w' 白，'.' 空。
  - 白先手。
  - 方向顺序 下/左/右/上，黑只允许前 k1 个，白只允许前 k2 个。
  - 走一格；目标格越界/障碍/己方棋子 -> 非法；目标格是对方棋子 -> 走子方立即获胜。
  - 黑棋走到 goal 行（默认最下面一行）-> 黑立即获胜。
  - 无合法走法：--no-move lose（默认，判负）或 draw。
  - 重复局面/循环：无法强制取胜的一方接受平局（retrograde 自然得到 DRAW）。

复杂度：状态数 = 2 * C(空格, 黑子数) * C(剩余, 白子数)。
  4x4 各 2 子约 2 万个状态，秒级；5x5 各 2 子约 17 万，几秒；
  再大就别用本脚本了，改用 C++ 对拍 + 抽样验证。
"""

import argparse
import random
import sys
from collections import deque

DIRS = [(1, 0), (0, -1), (0, 1), (-1, 0)]  # 下 左 右 上
DIR_NAME = "DLRU"


class Game:
    def __init__(self, n, m, k1, k2, obs=0, black=0, white=0,
                 goal="bottom", no_move="lose"):
        self.n, self.m = n, m
        self.k1, self.k2 = k1, k2
        self.obs, self.black, self.white = obs, black, white
        self.goal_row = (n - 1) if goal == "bottom" else 0
        self.no_move = no_move

    # ---------- 走法生成 ----------
    def gen(self, b, w, turn):
        """返回 (moves, has_win)。moves = [(nb, nw)]；has_win 表示存在立即获胜走法。"""
        own, opp = (b, w) if turn == 1 else (w, b)
        k = self.k1 if turn == 1 else self.k2
        moves = []
        o = own
        while o:
            lsb = o & -o
            i = lsb.bit_length() - 1
            o ^= lsb
            r, c = divmod(i, self.m)
            for d in range(k):
                dr, dc = DIRS[d]
                nr, nc = r + dr, c + dc
                if nr < 0 or nr >= self.n or nc < 0 or nc >= self.m:
                    continue
                bit = 1 << (nr * self.m + nc)
                if self.obs & bit:
                    continue
                if own & bit:
                    continue
                if opp & bit:                      # 踩踏：立即获胜
                    return [], True
                if turn == 1 and nr == self.goal_row:   # 黑棋突防
                    return [], True
                if turn == 1:
                    moves.append(((b ^ lsb) | bit, w))
                else:
                    moves.append((b, (w ^ lsb) | bit))
        return moves, False

    def legal_moves(self, b, w, turn):
        """给单测用的展开版：[(fr, fc, tr, tc, is_win)]"""
        out = []
        own, opp = (b, w) if turn == 1 else (w, b)
        k = self.k1 if turn == 1 else self.k2
        o = own
        while o:
            lsb = o & -o
            i = lsb.bit_length() - 1
            o ^= lsb
            r, c = divmod(i, self.m)
            for d in range(k):
                dr, dc = DIRS[d]
                nr, nc = r + dr, c + dc
                if nr < 0 or nr >= self.n or nc < 0 or nc >= self.m:
                    continue
                bit = 1 << (nr * self.m + nc)
                if (self.obs | own) & bit:
                    continue
                win = bool(opp & bit) or (turn == 1 and nr == self.goal_row)
                out.append((r, c, nr, nc, win))
        return sorted(out)


# ---------- 精确解：retrograde（逆推） ----------
def solve(game):
    b0, w0 = game.black, game.white
    start = (b0, w0, 0)          # 白先
    ids = {start: 0}
    order = [start]
    succ, win = [], []

    i = 0
    while i < len(order):
        b, w, t = order[i]
        mv, has_win = game.gen(b, w, t)
        if has_win:
            succ.append(())
            win.append(True)
        else:
            win.append(False)
            lst = []
            for nb, nw in mv:
                ns = (nb, nw, 1 - t)
                j = ids.get(ns)
                if j is None:
                    j = len(order)
                    ids[ns] = j
                    order.append(ns)
                lst.append(j)
            succ.append(tuple(lst))
        i += 1

    n = len(order)
    pred = [[] for _ in range(n)]
    for s, lst in enumerate(succ):
        for t in lst:
            pred[t].append(s)

    outdeg = [len(lst) for lst in succ]
    UNKNOWN, WIN, LOSE, DRAW = -2, 1, -1, 0
    res = [UNKNOWN] * n
    q = deque()

    for s in range(n):
        if win[s]:
            res[s] = WIN
            q.append(s)
        elif outdeg[s] == 0:
            if game.no_move == "lose":
                res[s] = LOSE
                q.append(s)
            else:
                res[s] = DRAW

    while q:
        s = q.popleft()
        if res[s] == LOSE:
            for p in pred[s]:
                if res[p] == UNKNOWN:
                    res[p] = WIN
                    q.append(p)
        else:  # WIN
            for p in pred[s]:
                if res[p] == UNKNOWN:
                    outdeg[p] -= 1
                    if outdeg[p] == 0:
                        res[p] = LOSE
                        q.append(p)

    r = res[0]
    if r == WIN:
        # 轮到白走且白必胜；轮到黑走且黑必胜
        winner = "WHITE" if start[2] == 0 else "BLACK"
    elif r == LOSE:
        winner = "BLACK" if start[2] == 0 else "WHITE"
    else:
        winner = "DRAW"
    stats = {
        "states": n,
        "win": sum(1 for x in res if x == WIN),
        "lose": sum(1 for x in res if x == LOSE),
        "draw": sum(1 for x in res if x in (DRAW, UNKNOWN)),
        "value": r,
    }
    return winner, stats, order, res


# ---------- 独立实现：不动点迭代（仅用于 selftest 交叉验证） ----------
def solve_fixpoint(game):
    b0, w0 = game.black, game.white
    start = (b0, w0, 0)
    ids = {start: 0}
    order = [start]
    succ, win = [], []
    i = 0
    while i < len(order):
        b, w, t = order[i]
        mv, has_win = game.gen(b, w, t)
        if has_win:
            succ.append(())
            win.append(True)
        else:
            win.append(False)
            lst = []
            for nb, nw in mv:
                ns = (nb, nw, 1 - t)
                j = ids.get(ns)
                if j is None:
                    j = len(order)
                    ids[ns] = j
                    order.append(ns)
                lst.append(j)
            succ.append(tuple(lst))
        i += 1

    val = [None] * len(order)
    for s in range(len(order)):
        if win[s]:
            val[s] = 1
        elif not succ[s]:
            val[s] = -1 if game.no_move == "lose" else 0

    changed = True
    while changed:
        changed = False
        for s in range(len(order)):
            if val[s] is not None:
                continue
            vals = [val[t] for t in succ[s]]
            if any(v == -1 for v in vals):
                val[s] = 1
                changed = True
            elif all(v == 1 for v in vals):
                val[s] = -1
                changed = True

    for s in range(len(order)):
        if val[s] is None:
            val[s] = 0
    r = val[0]
    winner = ("WHITE" if start[2] == 0 else "BLACK") if r == 1 else \
             (("BLACK" if start[2] == 0 else "WHITE") if r == -1 else "DRAW")
    return winner, {"states": len(order)}


# ---------- IO ----------
def parse_board(lines, n, m):
    obs = black = white = 0
    for r in range(n):
        row = lines[r].strip()
        if len(row) != m:
            raise ValueError(f"第 {r+1} 行长度 {len(row)} != {m}: {row!r}")
        for c, ch in enumerate(row):
            bit = 1 << (r * m + c)
            if ch == "#":
                obs |= bit
            elif ch == "b":
                black |= bit
            elif ch == "w":
                white |= bit
            elif ch != ".":
                raise ValueError(f"非法字符 {ch!r}")
    return obs, black, white


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int)
    ap.add_argument("--m", type=int)
    ap.add_argument("--k1", type=int, default=2)
    ap.add_argument("--k2", type=int, default=2)
    ap.add_argument("--goal", choices=["bottom", "top"], default="bottom")
    ap.add_argument("--no-move", choices=["lose", "draw"], default="lose")
    ap.add_argument("--moves", action="store_true", help="输出初始局面合法走法")
    ap.add_argument("--stats", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--file", help="从文件读入（第一行 n m，第二行 k1 k2，之后 n 行棋盘）")
    args = ap.parse_args()

    if args.selftest:
        random.seed(20251019)
        bad = 0
        for trial in range(300):
            n = random.randint(1, 3)
            m = random.randint(1, 3)
            cells = n * m
            if cells > 6:
                m = max(1, 6 // n)
                cells = n * m
            obs = black = white = 0
            free = list(range(cells))
            random.shuffle(free)
            p = 0
            for _ in range(random.randint(0, 2)):
                if p >= len(free):
                    break
                obs |= 1 << free[p]; p += 1
            nb = random.randint(0, 2)
            for _ in range(nb):
                if p >= len(free):
                    break
                black |= 1 << free[p]; p += 1
            nw = random.randint(0, 2)
            for _ in range(nw):
                if p >= len(free):
                    break
                white |= 1 << free[p]; p += 1
            g = Game(n, m, random.randint(1, 3), random.randint(1, 3),
                     obs, black, white, random.choice(["top", "bottom"]),
                     random.choice(["lose", "draw"]))
            a, sa, _, _ = solve(g)
            b, sb = solve_fixpoint(g)
            if a != b:
                bad += 1
                print("MISMATCH", n, m, a, b, sa, sb)
        print(f"selftest done, mismatches={bad}")
        return 0 if bad == 0 else 1

    if args.file:
        with open(args.file) as f:
            lines = [ln.rstrip("\n") for ln in f if ln.strip() != ""]
        n, m = map(int, lines[0].split())
        k1, k2 = map(int, lines[1].split())
        board = lines[2:2 + n]
    else:
        if not args.n or not args.m:
            ap.error("需要 --n/--m 或 --file")
        n, m = args.n, args.m
        k1, k2 = args.k1, args.k2
        print(f"请输入 {n} 行棋盘（. # b w）：", file=sys.stderr)
        board = [sys.stdin.readline() for _ in range(n)]

    obs, black, white = parse_board(board, n, m)
    g = Game(n, m, k1, k2, obs, black, white, args.goal, args.no_move)

    if args.moves:
        print("# from_r from_c to_r to_c win")
        for mv in g.legal_moves(black, white, 0):
            print(*[int(x) for x in mv])
        print("# black moves (turn=1):")
        for mv in g.legal_moves(black, white, 1):
            print(*[int(x) for x in mv])

    winner, stats, _, _ = solve(g)
    print(winner)
    if args.stats:
        print(stats, file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
