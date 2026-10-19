#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""随机生成训练用例。

用法：
  python3 gen.py --n 4 --m 4 --count 20 --out cases
  python3 gen.py --n 3 --m 3 --count 5            # 打到 stdout，用 === 分隔
"""

import argparse
import os
import random


def gen_case(n, m, k1, k2, pieces=1, obstacles=0, rng=None, goal_bottom=True):
    rng = rng or random.Random()
    total = n * m
    need = 2 * pieces + obstacles + 2
    if need > total:
        raise ValueError(f"{n}x{m} 放不下 {pieces} 黑 + {pieces} 白 + {obstacles} 障碍")
    cells = list(range(total))
    rng.shuffle(cells)
    obs = cells[:obstacles]
    rest = cells[obstacles:]
    black = rest[:pieces]
    white = rest[pieces:2 * pieces]

    grid = [["." for _ in range(m)] for _ in range(n)]
    for i in obs:
        grid[i // m][i % m] = "#"
    for i in black:
        grid[i // m][i % m] = "b"
    for i in white:
        grid[i // m][i % m] = "w"

    lines = [f"{n} {m}", f"{k1} {k2}"]
    lines += ["".join(row) for row in grid]
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=4)
    ap.add_argument("--m", type=int, default=4)
    ap.add_argument("--k1", type=int, default=None)
    ap.add_argument("--k2", type=int, default=None)
    ap.add_argument("--pieces", type=int, default=1, help="每方棋子数")
    ap.add_argument("--obstacles", type=int, default=0)
    ap.add_argument("--count", type=int, default=10)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--out", default=None, help="输出目录；不给则打到 stdout")
    args = ap.parse_args()

    rng = random.Random(args.seed)
    if args.out:
        os.makedirs(args.out, exist_ok=True)

    for i in range(args.count):
        k1 = args.k1 or rng.randint(1, 3)
        k2 = args.k2 or rng.randint(1, 3)
        n, m = args.n, args.m
        obs = args.obstacles
        if obs * 1 + args.pieces * 2 + 2 > n * m:
            obs = 0
        text = gen_case(n, m, k1, k2, args.pieces, obs, rng)
        if args.out:
            with open(os.path.join(args.out, f"case{i:03d}.txt"), "w") as f:
                f.write(text)
        else:
            print(text, end="")
            print("===")


if __name__ == "__main__":
    main()
