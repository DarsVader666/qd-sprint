#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Download learncpp lessons starting at 11.6, render EN + ZH (Google-translated) PDFs."""
import copy, json, os, re, subprocess, sys, time
import requests
from bs4 import BeautifulSoup, NavigableString, Tag

START = "https://www.learncpp.com/cpp-tutorial/constant-expressions/"
N_LESSONS = 1
OUT = os.path.dirname(os.path.abspath(__file__))
UA = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146 Safari/537.36"

sess = requests.Session()
sess.headers["User-Agent"] = UA


def fetch(url):
    for i in range(4):
        try:
            r = sess.get(url, timeout=40)
            if r.status_code == 200:
                return r.text
            time.sleep(2 * (i + 1))
        except Exception:
            time.sleep(2 * (i + 1))
    raise RuntimeError("fetch failed: " + url)


def collect_lessons():
    lessons, url = [], START
    for _ in range(N_LESSONS):
        html = fetch(url)
        soup = BeautifulSoup(html, "lxml")
        title = soup.title.string.split(" – ")[0].strip()
        content = soup.select_one("div.entry-content")
        if content is None:
            raise RuntimeError("no entry-content at " + url)
        lessons.append({"url": url, "title": title, "content": str(content)})
        nxt = None
        for a in soup.find_all("a"):
            if "Next lesson" in a.get_text(" ", strip=True):
                nxt = a["href"]
                break
        if not nxt:
            break
        url = nxt
    return lessons


DROP_SELECTORS = [
    "script", "style", "iframe", "video", "audio", "ins",
    ".sharedaddy", ".jp-relatedposts", ".comment-respond", ".comments-area",
    "div[class*=advert]", "div[id*=advert]", ".adsbygoogle",
    "noscript", "form", ".wp-block-embed",
]


def clean_content(html):
    soup = BeautifulSoup(html, "lxml")
    for sel in DROP_SELECTORS:
        for t in soup.select(sel):
            t.decompose()
    # remove next/prev lesson nav
    for a in soup.find_all("a"):
        t = a.get_text(" ", strip=True)
        if t.startswith("Next lesson") or t.startswith("Previous lesson") or "Back to table of contents" in t:
            p = a.parent
            a.decompose()
            if p is not None and not p.get_text(strip=True) and p.name == "div":
                p.decompose()
    # drop empty divs/paragraphs
    for t in soup.find_all(["div", "p"]):
        if not t.get_text(strip=True) and not t.find(["img", "pre", "table"]):
            t.decompose()
    # unwrap redundant single-child divs
    for _ in range(3):
        changed = False
        for d in soup.find_all("div"):
            kids = [c for c in d.children if isinstance(c, Tag)]
            if len(kids) == 1 and not d.get("class"):
                d.unwrap()
                changed = True
        if not changed:
            break
    return soup


CSS = """
@page { size: A4; margin: 15mm 13mm; }
html, body { margin: 0; padding: 0; }
body { font: 11.2pt/1.6 "DejaVu Serif", Georgia, "Noto Serif CJK SC", "AR PL UMing CN", serif; color: #111; }
.lesson { page-break-before: always; }
.lesson:first-of-type { page-break-before: avoid; }
h1 { font-size: 17pt; margin: 0 0 4pt; line-height: 1.3; }
.meta { font-size: 8.5pt; color: #777; margin: 0 0 14pt; border-bottom: 1px solid #ddd; padding-bottom: 6pt; }
h2 { font-size: 13.5pt; margin: 16pt 0 6pt; }
h3 { font-size: 12pt; margin: 12pt 0 5pt; }
h4 { font-size: 11pt; margin: 10pt 0 4pt; }
p { margin: 6pt 0; }
ul, ol { margin: 6pt 0 6pt 0; padding-left: 20pt; }
li { margin: 3pt 0; }
pre { font: 9pt/1.45 "DejaVu Sans Mono", "Consolas", monospace; background: #f5f5f5;
      border: 1px solid #e0e0e0; border-radius: 3px; padding: 7pt 8pt; margin: 7pt 0;
      white-space: pre-wrap; word-wrap: break-word; page-break-inside: avoid; }
code { font-family: "DejaVu Sans Mono", "Consolas", monospace; font-size: 0.9em;
       background: #f0f0f0; padding: 0 1.5pt; border-radius: 2px; }
pre code { background: none; padding: 0; }
table { border-collapse: collapse; margin: 8pt 0; font-size: 10pt; }
th, td { border: 1px solid #ccc; padding: 4pt 6pt; vertical-align: top; }
a { color: #0b5fbf; text-decoration: none; }
blockquote { margin: 8pt 0; padding: 6pt 10pt; border-left: 3px solid #ccc; background: #fafafa; }
img { max-width: 100%; }
"""


def build_html(lessons, lang, title):
    parts = ["<!doctype html><html><head><meta charset='utf-8'>",
             "<title>%s</title><style>%s</style></head><body>" % (title, CSS)]
    parts.append("<h1 style='page-break-before:avoid;font-size:20pt'>%s</h1>" % title)
    parts.append("<p class='meta'>LearnCpp · %d lessons · %s</p>" % (len(lessons), lang))
    for L in lessons:
        parts.append("<section class='lesson'>")
        parts.append("<h1>%s</h1>" % L["title"])
        parts.append("<p class='meta'>%s</p>" % L["url"])
        parts.append(L["content"])
        parts.append("</section>")
    parts.append("</body></html>")
    return "\n".join(parts)


# ---------------- translation ----------------

BING_BASE = "https://cn.bing.com"


class Bing:
    def __init__(self):
        self.s = requests.Session()
        self.s.headers.update({"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                               "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36"})
        self.count = 0
        self.fail = 0
        self.refresh()

    def refresh(self):
        for _ in range(3):
            try:
                r = self.s.get(BING_BASE + "/translator", timeout=25)
                self.ig = re.search(r'IG:"([^"]+)"', r.text).group(1)
                key, self.token = re.search(r'params_AbusePreventionHelper\s*=\s*\[(\d+),"([^"]+)"', r.text).groups()
                self.iid = re.search(r'data-iid="([^"]+)"', r.text).group(1)
                self.url = "%s/ttranslatev3?isVertical=1&&IG=%s&IID=%s" % (BING_BASE, self.ig, self.iid)
                self.key = key
                self.count = 0
                return True
            except Exception:
                time.sleep(1.5)
        return False

    def translate(self, text):
        for attempt in range(2):
            try:
                if self.count >= 40:
                    self.refresh()
                data = {"fromLang": "en", "to": "zh-Hans", "text": text,
                        "token": self.token, "key": self.key}
                r = self.s.post(self.url, data=data, timeout=25,
                                headers={"Referer": BING_BASE + "/translator", "Origin": BING_BASE})
                self.count += 1
                if r.status_code == 200 and r.text.strip():
                    return r.json()[0]["translations"][0]["text"]
                time.sleep(0.4)
                self.refresh()
            except Exception:
                time.sleep(0.4)
                try:
                    self.refresh()
                except Exception:
                    pass
        return None


BING = None
MARK = re.compile(r"\[\[\s*(\d+)\s*\]\]")
INLINE_TAGS = {"a", "code", "strong", "em", "b", "i", "kbd", "var", "sub", "sup", "u", "s", "span", "abbr"}
BLOCK_CHILD = {"p", "ul", "ol", "pre", "table", "div", "blockquote", "h1", "h2", "h3", "h4", "h5", "h6", "figure", "dl"}


def gtranslate(text):
    global BING
    text = text.strip()
    if not text or not re.search(r"[A-Za-z]", text):
        return text
    if BING is None:
        BING = Bing()
    if len(text) <= 4000:
        return BING.translate(text)
    # split long text
    out = []
    cur = ""
    for piece in re.split(r"(?<=[.!?])\s+", text):
        if len(cur) + len(piece) + 1 > 3800 and cur:
            out.append(cur)
            cur = piece
        else:
            cur = (cur + " " + piece).strip()
    if cur:
        out.append(cur)
    res = []
    for c in out:
        t = BING.translate(c)
        if t is None:
            return None
        res.append(t)
        time.sleep(0.1)
    return " ".join(res)


def translate_block(tag):
    # protect inline children with markers
    inline = []

    def repl(node):
        k = len(inline)
        inline.append(str(node))
        return "[[%d]]" % k

    pieces = []
    for child in list(tag.children):
        if isinstance(child, NavigableString):
            pieces.append(str(child))
        elif isinstance(child, Tag) and child.name in INLINE_TAGS:
            pieces.append(repl(child))
        else:
            pieces.append(child.get_text(" ", strip=False))
    src = re.sub(r"\s+", " ", "".join(pieces)).strip()
    if not re.search(r"[A-Za-z]{2}", src):
        return
    tr = gtranslate(src)
    if tr is None or len(MARK.findall(tr)) != len(inline):
        # fallback: translate plain text, drop inline markup
        plain = re.sub(r"\[\[\s*\d+\s*\]\]", "", src)
        tr2 = gtranslate(plain)
        if tr2 is None:
            return
        tag.clear()
        tag.append(NavigableString(tr2))
        return
    # rebuild
    tag.clear()
    pos = 0
    for m in MARK.finditer(tr):
        if m.start() > pos:
            tag.append(NavigableString(tr[pos:m.start()]))
        idx = int(m.group(1))
        frag = BeautifulSoup(inline[idx], "lxml")
        for c in list(frag.contents):
            tag.append(c.extract())
        pos = m.end()
    if pos < len(tr):
        tag.append(NavigableString(tr[pos:]))


def translate_soup(soup):
    targets = []
    for tag in soup.find_all(["p", "li", "h1", "h2", "h3", "h4", "h5", "dd", "dt", "figcaption", "td", "th"]):
        if tag.find_parent(["pre", "code"]):
            continue
        if tag.name == "li" and tag.find(BLOCK_CHILD):
            continue
        if tag.name in ("td", "th") and tag.find(BLOCK_CHILD):
            continue
        targets.append(tag)
    total = len(targets)
    for i, tag in enumerate(targets, 1):
        translate_block(tag)
        if i % 25 == 0:
            print("  translated %d/%d" % (i, total), flush=True)
    # translate lesson titles
    return soup


def render_pdf(html_path, pdf_path):
    args = ["google-chrome", "--headless=new", "--disable-gpu", "--no-sandbox",
            "--no-pdf-header-footer", "--virtual-time-budget=20000",
            "--print-to-pdf=" + os.path.abspath(pdf_path), "file://" + os.path.abspath(html_path)]
    r = subprocess.run(args, capture_output=True, text=True, timeout=180)
    if not os.path.exists(pdf_path):
        sys.stderr.write(r.stdout[-2000:] + "\n" + r.stderr[-2000:] + "\n")
        raise RuntimeError("pdf render failed")


def main():
    print("collecting lessons...", flush=True)
    raw = collect_lessons()
    lessons = []
    for L in raw:
        soup = clean_content(L["content"])
        lessons.append({"url": L["url"], "title": L["title"], "soup": soup})
        print("  got", L["title"], flush=True)

    # EN
    if os.environ.get("SKIP_EN"):
        print("skip EN", flush=True)
    else:
        build_en(lessons)

    build_zh(lessons)


def build_en(lessons):
    en = [{"url": L["url"], "title": L["title"], "content": str(L["soup"])} for L in lessons]
    en_title = "LearnCpp 5.5 Constant Expressions (EN)"
    en_html = os.path.join(OUT, "en.html")
    open(en_html, "w", encoding="utf-8").write(build_html(en, "English", en_title))
    render_pdf(en_html, os.path.join(OUT, "learncpp_5.5_constexpr_EN.pdf"))
    print("EN pdf done", flush=True)


def build_zh(lessons):
    # ZH
    zh_lessons = []
    for L in lessons:
        soup = copy.deepcopy(L["soup"])
        print("translating", L["title"], flush=True)
        translate_soup(soup)
        zt = gtranslate(L["title"]) or L["title"]
        zh_lessons.append({"url": L["url"], "title": zt, "content": str(soup)})
    zh_title = "LearnCpp 5.5 常量表达式（中文机翻）"
    zh_html = os.path.join(OUT, "zh.html")
    open(zh_html, "w", encoding="utf-8").write(build_html(zh_lessons, "中文（Bing 机翻）", zh_title))
    render_pdf(zh_html, os.path.join(OUT, "learncpp_5.5_constexpr_ZH.pdf"))
    print("ZH pdf done", flush=True)


if __name__ == "__main__":
    main()
