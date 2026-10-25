#!/usr/bin/env python3
"""Check that the string-to-XmString converter validates XrmValue inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("CvtStringToXmString(\n        Display *display")
body = text[start : start + 850]
for needle in ("from == NULL", "from->addr == NULL", "to == NULL"):
    if needle not in body:
        raise SystemExit(f"XmString converter guard is missing: {needle}")

print("XmString converter input guards present")
