#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把 deck.csv 转成 AnkiDroid 可直接导入的 .apkg。

用法：
    pip install genanki
    python3 anki/make_apkg.py
    python3 anki/make_apkg.py --csv anki/deck.csv --out anki/QD-CPP.apkg

说明：
  - CSV 里的 #separator/#html/#tags column 是桌面 Anki 的导入指令；
    AnkiDroid 对 CSV 支持不稳定，直接导入 .apkg 最省事。
  - 每次改完 deck.csv，重新跑一次这个脚本，覆盖旧的 .apkg。
"""

import argparse
import csv
import html
from pathlib import Path

import genanki

HERE = Path(__file__).resolve().parent
MODEL_ID = 1734567890
DECK_ID = 2059400110


def load_rows(path: Path):
    rows = []
    with path.open(encoding="utf-8") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            rows.extend(csv.reader([line]))
    for i, row in enumerate(rows):
        if len(row) != 3:
            raise ValueError(f"第 {i+1} 行不是 3 列: {row}")
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default=str(HERE / "deck.csv"))
    ap.add_argument("--out", default=str(HERE / "QD-CPP.apkg"))
    ap.add_argument("--deck", default="QD-C++")
    args = ap.parse_args()

    rows = load_rows(Path(args.csv))

    model = genanki.Model(
        MODEL_ID,
        "QD Front/Back (Chinese)",
        fields=[{"name": "Front"}, {"name": "Back"}],
        templates=[{
            "name": "Card 1",
            "qfmt": '<div class="front">{{Front}}</div>',
            "afmt": ('<div class="front">{{Front}}</div>'
                     '<hr id="answer"><div class="back">{{Back}}</div>'),
        }],
        css="""
        .card { font-family: -apple-system, "PingFang SC", "Noto Sans CJK SC", sans-serif;
                font-size: 20px; line-height: 1.6; color: #222; background: #fff; }
        .front { font-weight: 600; }
        .back { color: #0b5; }
        """,
    )

    deck = genanki.Deck(DECK_ID, args.deck)
    for front, back, tags in rows:
        deck.add_note(genanki.Note(
            model=model,
            fields=[html.escape(front, quote=False), html.escape(back, quote=False)],
            tags=tags.split(),
        ))

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    genanki.Package(deck).write_to_file(out)
    print(f"wrote {out} ({len(rows)} notes, deck={args.deck})")


if __name__ == "__main__":
    main()
