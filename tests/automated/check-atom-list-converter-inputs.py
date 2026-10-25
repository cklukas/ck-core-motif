#!/usr/bin/env python3
"""Check that atom-list conversion validates display and XrmValue inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.rfind("CvtStringToAtomList(")
start = text.index("{", start)
body = text[start : start + 850]
for needle in ("dpy == NULL", "from == NULL", "from->addr == NULL", "to == NULL"):
    if needle not in body:
        raise SystemExit(f"atom-list converter guard is missing: {needle}")

print("atom-list converter input guards present")
