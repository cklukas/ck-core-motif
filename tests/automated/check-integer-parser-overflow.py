#!/usr/bin/env python3
"""Check that resource integer parsing rejects overflow before arithmetic."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.rfind("isInteger(\n")
body = text[start : start + 1800]
for needle in ("unsigned long val", "limit - digit", "INT_MIN"):
    if needle not in body:
        raise SystemExit(f"integer overflow handling is missing: {needle}")
if "val *= 10" in body:
    raise SystemExit("integer parser still multiplies without a bound")

print("integer parser overflow handling present")
