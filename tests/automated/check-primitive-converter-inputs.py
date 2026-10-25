#!/usr/bin/env python3
"""Check primitive string converters validate source and destination values."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("CvtStringToChar(", "CvtStringToKeySym("):
    start = text.rfind(name)
    body = text[start : start + 700]
    if "from == NULL" not in body or "to == NULL" not in body:
        raise SystemExit(f"primitive converter guard is missing: {name}")
if "from->size < sizeof(unsigned char)" not in text:
    raise SystemExit("character converter size guard is missing")

print("primitive converter input guards present")
