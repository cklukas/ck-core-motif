#!/usr/bin/env python3
"""Check pixel/rendition converters and integer parsing validate inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.rfind("isInteger(\n")
if "string == NULL || value == NULL" not in text[start : start + 500]:
    raise SystemExit("integer parser input guard is missing")
for name in ("CvtStringToRenditionPixel(", "CvtPixelToRenditionPixel("):
    start = text.rfind(name)
    body = text[start : start + 1100]
    if "num_args == NULL" not in body or "to_val == NULL" not in body:
        raise SystemExit(f"pixel converter guard is missing: {name}")

print("pixel converter input guards present")
