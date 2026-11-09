#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把一组 .zh.md（人工/子智能体精译）拼成一份中文 PDF。

用法：
    python3 build_zh_pdf.py --out /tmp/out.pdf --title "LearnCpp 第25章 虚函数" \
        --subtitle "人工精译 · 删废话 · 代码逐字节保留 · 术语统一" \
        /tmp/tr/25.1.zh.md /tmp/tr/25.2.zh.md ...

依赖：python3 + google-chrome（headless）
"""
import argparse, html, os, re, subprocess, sys

CSS = """
@page { size: A4; margin: 16mm 14mm; }
body { font-family:"Noto Sans CJK SC","Source Han Sans SC","WenQuanYi Zen Hei",sans-serif;
       font-size:10.5pt; line-height:1.75; color:#1b1b1b; }
h1 { font-size:17pt; margin:0 0 10pt; padding-bottom:6pt; border-bottom:2px solid #333;
     page-break-before:always; }
h2 { font-size:13pt; margin:16pt 0 6pt; color:#0a3d62; }
h3 { font-size:11.5pt; margin:12pt 0 4pt; }
h4 { font-size:10.8pt; margin:10pt 0 4pt; color:#333; }
p { margin:6pt 0; text-align:justify; }
pre { background:#f5f6f8; border-left:3px solid #4a90d9; padding:8pt 10pt; margin:8pt 0;
      font-family:"DejaVu Sans Mono",monospace; font-size:8.6pt; line-height:1.5;
      white-space:pre-wrap; word-break:break-word; page-break-inside:avoid; }
code { font-family:"DejaVu Sans Mono",monospace; font-size:9pt; background:#eef1f4;
       padding:1px 3px; border-radius:2px; }
pre code { background:none; padding:0; font-size:8.6pt; }
ul,ol { margin:6pt 0 6pt 18pt; } li { margin:3pt 0; }
table { border-collapse:collapse; margin:10pt 0; font-size:10pt; page-break-inside:avoid; }
th,td { border:1px solid #bbb; padding:5pt 10pt; text-align:left; }
th { background:#eef3f9; font-weight:600; }
hr { border:none; border-top:1px solid #ddd; margin:14pt 0; }
blockquote { margin:8pt 0; padding:6pt 10pt; background:#f0f6ff; border-left:3px solid #4a90d9; }
.cover { text-align:center; padding-top:60mm; page-break-after:always; }
.cover h1 { border:none; page-break-before:avoid; font-size:26pt; margin:0; }
.cover p { color:#666; font-size:11pt; }
.toc { page-break-after:always; }
.toc h2 { border-bottom:1px solid #ccc; padding-bottom:4pt; }
.toc ol { line-height:2.0; font-size:11pt; }
"""


def inline(s):
    s = html.escape(s)
    s = re.sub(r'`([^`]+)`', r'<code>\1</code>', s)
    s = re.sub(r'\*\*([^*]+)\*\*', r'<strong>\1</strong>', s)
    s = re.sub(r'(?<!\*)\*([^*]+)\*(?!\*)', r'<em>\1</em>', s)
    return s


def md2html(md):
    md = re.sub(r'\n---\n\n(下一课|Next lesson)[^\n]*\n?', '\n', md)
    lines = md.split('\n')
    out, para, tbl = [], [], []
    in_code, list_tag = False, None

    def flush_para():
        nonlocal para
        if para:
            out.append('<p>' + inline(' '.join(para)) + '</p>')
            para = []

    def close_list():
        nonlocal list_tag
        if list_tag:
            out.append(f'</{list_tag}>')
            list_tag = None

    def flush_table():
        nonlocal tbl
        if not tbl:
            return
        rows, tbl = tbl, []
        cells = [[c.strip() for c in r.strip().strip('|').split('|')] for r in rows]
        if len(cells) >= 2 and all(re.fullmatch(r':?-{2,}:?', c) for c in cells[1]):
            out.append('<table><thead><tr>'
                       + ''.join(f'<th>{inline(c)}</th>' for c in cells[0])
                       + '</tr></thead><tbody>')
            for r in cells[2:]:
                out.append('<tr>' + ''.join(f'<td>{inline(c)}</td>' for c in r) + '</tr>')
            out.append('</tbody></table>')
        else:
            for r in cells:
                out.append('<p>' + inline(' | '.join(r)) + '</p>')

    for ln in lines:
        st = ln.strip()
        if st.startswith('```'):
            flush_para(); flush_table(); close_list()
            out.append('<pre><code>' if not in_code else '</code></pre>')
            in_code = not in_code
            continue
        if in_code:
            out.append(html.escape(ln))
            continue
        if st.startswith('|') and st.endswith('|'):
            flush_para(); close_list(); tbl.append(st); continue
        flush_table()
        if not st:
            flush_para(); close_list(); continue
        m = re.match(r'^(#{1,4})\s+(.*)$', st)
        if m:
            flush_para(); close_list()
            lv = len(m.group(1))
            out.append(f'<h{lv}>{inline(m.group(2))}</h{lv}>')
            continue
        if re.match(r'^---+\s*$', st):
            flush_para(); close_list(); out.append('<hr>'); continue
        m = re.match(r'^[-*]\s+(.*)$', st)
        if m:
            flush_para()
            if list_tag != 'ul':
                close_list(); out.append('<ul>'); list_tag = 'ul'
            out.append('<li>' + inline(m.group(1)) + '</li>')
            continue
        m = re.match(r'^(\d+)\.\s+(.*)$', st)
        if m:
            flush_para()
            if list_tag != 'ol':
                close_list(); out.append('<ol>'); list_tag = 'ol'
            out.append('<li>' + inline(m.group(2)) + '</li>')
            continue
        if st.startswith('>'):
            flush_para(); close_list()
            out.append('<blockquote>' + inline(st.lstrip('> ')) + '</blockquote>')
            continue
        para.append(st)
    flush_para(); flush_table(); close_list()
    if in_code:
        out.append('</code></pre>')
    return '\n'.join(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('files', nargs='+')
    ap.add_argument('--out', required=True)
    ap.add_argument('--title', default='LearnCpp 中文精译')
    ap.add_argument('--subtitle', default='人工精译 · 删废话 · 代码逐字节保留 · 术语统一')
    a = ap.parse_args()

    parts, toc = [], []
    for f in a.files:
        md = open(f, encoding='utf-8').read()
        title = md.split('\n', 1)[0].lstrip('# ').strip()
        toc.append(title)
        parts.append(f'<section>{md2html(md)}</section>')

    toc_html = ('<div class="toc"><h2>目录</h2><ol>'
                + ''.join(f'<li>{html.escape(t)}</li>' for t in toc)
                + '</ol></div>')
    doc = (f'<!DOCTYPE html><html lang="zh-CN"><head><meta charset="utf-8">'
           f'<title>{html.escape(a.title)}</title><style>{CSS}</style></head><body>'
           f'<div class="cover"><h1>{html.escape(a.title)}</h1>'
           f'<p>{html.escape(a.subtitle)}</p>'
           f'<p style="margin-top:30mm;color:#999">共 {len(a.files)} 节</p></div>'
           f'{toc_html}{"".join(parts)}</body></html>')

    html_path = os.path.splitext(a.out)[0] + '.html'
    open(html_path, 'w', encoding='utf-8').write(doc)

    subprocess.run(['google-chrome', '--headless=new', '--disable-gpu', '--no-sandbox',
                    '--no-pdf-header-footer', '--virtual-time-budget=20000',
                    f'--print-to-pdf={os.path.abspath(a.out)}',
                    'file://' + os.path.abspath(html_path)],
                   capture_output=True, text=True, timeout=180)
    if not os.path.exists(a.out):
        sys.exit('PDF 渲染失败')
    print(f'OK: {a.out}  ({os.path.getsize(a.out)//1024} KB, {len(a.files)} 节, '
          f'{doc.count("<table>")} 张表)')


if __name__ == '__main__':
    main()
