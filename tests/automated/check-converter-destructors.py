#!/usr/bin/env python3
"""Check converter destructors tolerate missing destination values."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("CvtStringToXmStringDestroy(", "CvtStringToXmFontListDestroy(",
             "XmStringCvtDestroy(", "StringCvtDestroy(",
             "CardinalListCvtDestroy(", "SimpleDestructor(",
             "CvtStringToXmTabListDestroy(", "CvtStringToXmRenderTableDestroy("):
    start = text.rfind(name)
    body = text[start : start + 700]
    if "to == NULL || to->addr == NULL" not in body:
        raise SystemExit(f"converter destructor guard is missing: {name}")

print("converter destructor guards present")
