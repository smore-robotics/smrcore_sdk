#!/usr/bin/env python3
"""Write a 2-level outline (bookmarks) into the merged docs PDF.

Usage:
    pdf_bookmarks.py OUT_PDF MANIFEST

MANIFEST is a TSV produced by docs_pdf.sh, one line per merged part:
    <part_pdf>\t<source_html>

Page counts come from each part PDF (so the cumulative start page is exact),
and bookmark titles come from each source HTML <title>.
"""

import html
import re
import sys
from pathlib import Path

from pypdf import PdfReader, PdfWriter

GROUP_GUIDE = "用户指南"
GROUP_API = "C++ API 参考"


def title_from_html(path: str) -> str:
    data = Path(path).read_text(encoding="utf-8", errors="replace")
    match = re.search(r"<title>(.*?)</title>", data, re.S)
    text = html.unescape(match.group(1)).strip() if match else Path(path).stem
    text = re.sub(r"^smrcore_sdk C\+\+ API:\s*", "", text)
    text = re.sub(r"\s*[-|]\s*smrcore_sdk.*$", "", text)
    return text or Path(path).stem


def group_of(html_path: str) -> str:
    return GROUP_API if "/api/cpp/" in html_path else GROUP_GUIDE


def main() -> int:
    out_pdf, manifest = sys.argv[1], sys.argv[2]

    entries = []
    for line in Path(manifest).read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        part, src = line.split("\t", 1)
        entries.append((part, src))

    writer = PdfWriter(clone_from=out_pdf)

    start = 0
    current_group = None
    parent = None
    for part, src in entries:
        pages = len(PdfReader(part).pages)
        name = Path(src).name
        if name in ("cover.html", "toc.html"):
            writer.add_outline_item(title_from_html(src), start)
            current_group = None
            parent = None
            start += pages
            continue
        group = group_of(src)
        if group != current_group:
            parent = writer.add_outline_item(group, start)
            current_group = group
        writer.add_outline_item(title_from_html(src), start, parent=parent)
        start += pages

    with open(out_pdf, "wb") as fh:
        writer.write(fh)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
