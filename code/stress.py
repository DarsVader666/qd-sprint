#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""对拍脚本：oracle.py（精确解） vs 你的 C++ 程序。

用法：
  python3 stress.py --bin ./stub --count 50
  python3 stress.py --bin ./stub --count 100 --moves      # 只对拍初始走法
  python3 stress.py --bin ./my --n 4 --pieces 2 --max-states 50000

判定：
  - 结果模式：比较第一行 WHITE/BLACK/DRAW。
  - --moves 模式：比较初始局面（白方）的合法走法集合（含 win 标记）。
  - 崩溃 / 超时 / 输出为空 都算失败。

注意：你的程序必须是“精确搜索”（不能有 depth 上限），否则结果模式和
      精确解对不上是正常的 —— 那种情况先用 --moves 对拍规则，再在
      小棋盘上做精确搜索。
"""

import argparse
import ast
import os
import random
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ORACLE = os.path.join(HERE, "oracle.py")


def run(cmd, inp=None, timeout=30):
    try:
        p = subprocess.run(cmd, input=inp, capture_output=True, text=True, timeout=timeout)
        return p.returncode, p.stdout, p.stderr
    except subprocess.TimeoutExpired:
        return "TIMEOUT", "", ""


def oracle_result(path, timeout):
    rc, out, err = run([sys.executable, ORACLE, "--file", path, "--stats"], timeout=timeout)
    if rc != 0:
        raise RuntimeError(f"oracle failed rc={rc}: {err[:300]}")
    result = out.strip().splitlines()[0].strip()
    states = None
    try:
        states = ast.literal_eval(err.strip().splitlines()[-1])["states"]
    except Exception:
        pass
    return result, states


def oracle_moves(path, timeout, turn=0):
    rc, out, _ = run([sys.executable, ORACLE, "--file", path, "--moves"], timeout=timeout)
    if rc != 0:
        raise RuntimeError("oracle --moves failed")
    moves = set()
    section = 0
    for line in out.splitlines():
        line = line.strip()
        if line.startswith("# black moves"):
            section = 1
            continue
        if line.startswith("#") or not line:
            continue
        if section != turn:
            continue
        parts = line.split()
        if len(parts) == 5:
            moves.add(tuple(int(x) for x in parts))
    return moves


def stub_moves(path, binpath, timeout):
    rc, out, err = run([binpath, "--moves"], inp=open(path).read(), timeout=timeout)
    if rc != 0:
        raise RuntimeError(f"bin crashed rc={rc}: {err[:300]}")
    moves = set()
    for line in out.splitlines():
        line = line.strip()
        if line.startswith("#") or not line:
            continue
        parts = line.split()
        if len(parts) == 5:
            moves.add(tuple(int(x) for x in parts))
    return moves


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bin", default="./stub")
    ap.add_argument("--count", type=int, default=50)
    ap.add_argument("--n", type=int, default=None, help="固定 n（默认 3~4 随机）")
    ap.add_argument("--m", type=int, default=None)
    ap.add_argument("--pieces", type=int, default=None, help="固定每方子数（默认 1~2 随机）")
    ap.add_argument("--moves", action="store_true")
    ap.add_argument("--seed", type=int, default=20251019)
    ap.add_argument("--max-states", type=int, default=200000)
    ap.add_argument("--timeout", type=float, default=20)
    ap.add_argument("--keep", action="store_true", help="保留失败用例到 fail/")
    args = ap.parse_args()

    sys.path.insert(0, HERE)
    from gen import gen_case

    rng = random.Random(args.seed)
    fails = 0
    skipped = 0
    tested = 0

    for t in range(args.count):
        n = args.n or rng.choice([3, 3, 4, 4])
        m = args.m or rng.choice([3, 3, 4])
        pieces = args.pieces or rng.choice([1, 1, 2])
        obs = 0
        if n * m >= 12 and rng.random() < 0.4:
            obs = rng.randint(1, 2)
        try:
            text = gen_case(n, m, rng.randint(1, 3), rng.randint(1, 3),
                            pieces, obs, rng)
        except ValueError:
            continue

        fd, path = tempfile.mkstemp(suffix=".txt", prefix="case_")
        os.close(fd)
        with open(path, "w") as f:
            f.write(text)

        try:
            if args.moves:
                exp = oracle_moves(path, args.timeout)
                got = stub_moves(path, args.bin, args.timeout)
                ok = exp == got
                detail = f"期望 {len(exp)} 步, 得到 {len(got)} 步"
            else:
                exp, states = oracle_result(path, args.timeout)
                if states and states > args.max_states:
                    skipped += 1
                    continue
                rc, out, err = run([args.bin], inp=text, timeout=args.timeout)
                got = out.strip().splitlines()[0].strip() if out.strip() else "<empty>"
                ok = (rc == 0 and got == exp)
                detail = f"期望 {exp}, 得到 {got} (rc={rc})"
        except Exception as e:
            ok = False
            detail = f"异常: {e}"

        tested += 1
        if not ok:
            fails += 1
            if fails <= 5:
                print(f"[FAIL] case {t} ({n}x{m}, {pieces}子): {detail}")
                print(text)
            if args.keep:
                os.makedirs(os.path.join(HERE, "fail"), exist_ok=True)
                os.rename(path, os.path.join(HERE, "fail", f"fail_{t}.txt"))
                continue
        if os.path.exists(path):
            os.unlink(path)

    print(f"\n对拍结束: tested={tested}, fails={fails}, skipped={skipped}")
    return 0 if fails == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
