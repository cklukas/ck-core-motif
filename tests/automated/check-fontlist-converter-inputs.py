#!/usr/bin/env python3
"""Check that the font-list converter validates XrmValue inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("CvtStringToXmFontList(\n        Display *dpy")
body = text[start : start + 1000]
for needle in ("from == NULL", "from->addr == NULL", "to == NULL"):
    if needle not in body:
        raise SystemExit(f"font-list converter guard is missing: {needle}")

print("font-list converter input guards present")
