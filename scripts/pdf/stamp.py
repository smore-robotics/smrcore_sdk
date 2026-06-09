#!/usr/bin/env python3
"""Finalize the merged docs PDF: strip link annotations and stamp a footer.

Usage:
    stamp.py OUT_PDF [SKIP] [LOGO]

- Removes every /Link annotation. Chrome turns the HTML's relative links into
  PDF link annotations that point at local ``file://`` paths; for an offline
  manual those are useless and leak local paths, so we drop them. Document
  bookmarks/outline are unaffected (they live in /Outlines, not page /Annots).
- Stamps a thin footer (logo + brand + page number) on content pages. SKIP
  front-matter pages (default 2: cover + TOC) stay clean. Numbering starts at 1
  on the first content page.
"""

import io
import os
import sys

from pypdf import PdfReader, PdfWriter
from pypdf.generic import ArrayObject, NameObject
from reportlab.lib.colors import Color
from reportlab.lib.utils import ImageReader
from reportlab.pdfgen import canvas

MARGIN = 45.0  # ~16 mm, matches the @page margin used for the HTML pages.
FOOTER_Y = 30.0
LOGO_H = 13.0
BRAND = "smrcore_sdk"
GRAY = Color(0.42, 0.46, 0.52)
RULE = Color(0.82, 0.85, 0.89)


def strip_links(page) -> None:
    annots = page.get("/Annots")
    if not annots:
        return
    kept = ArrayObject(
        a for a in annots if a.get_object().get("/Subtype") != "/Link"
    )
    if kept:
        page[NameObject("/Annots")] = kept
    else:
        del page[NameObject("/Annots")]


def overlay(width: float, height: float, number: int, logo) -> "PdfReader":
    buf = io.BytesIO()
    c = canvas.Canvas(buf, pagesize=(width, height))
    c.setStrokeColor(RULE)
    c.setLineWidth(0.6)
    c.line(MARGIN, FOOTER_Y + 10, width - MARGIN, FOOTER_Y + 10)

    text_x = MARGIN
    if logo is not None:
        iw, ih = logo.getSize()
        lw = LOGO_H * iw / ih
        c.drawImage(
            logo, MARGIN, FOOTER_Y - 3, width=lw, height=LOGO_H,
            mask="auto", preserveAspectRatio=True,
        )
        text_x = MARGIN + lw + 6

    c.setFillColor(GRAY)
    c.setFont("Helvetica", 8)
    c.drawString(text_x, FOOTER_Y, BRAND)
    c.drawRightString(width - MARGIN, FOOTER_Y, str(number))
    c.showPage()
    c.save()
    buf.seek(0)
    return PdfReader(buf)


def main() -> int:
    out_pdf = sys.argv[1]
    skip = int(sys.argv[2]) if len(sys.argv) > 2 else 2
    logo_path = sys.argv[3] if len(sys.argv) > 3 else ""
    logo = ImageReader(logo_path) if logo_path and os.path.isfile(logo_path) else None

    reader = PdfReader(out_pdf)
    writer = PdfWriter(clone_from=out_pdf)

    number = 0
    for idx, page in enumerate(writer.pages):
        strip_links(page)
        if idx < skip:
            continue
        number += 1
        box = reader.pages[idx].mediabox
        stamp = overlay(float(box.width), float(box.height), number, logo)
        page.merge_page(stamp.pages[0])

    with open(out_pdf, "wb") as fh:
        writer.write(fh)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
