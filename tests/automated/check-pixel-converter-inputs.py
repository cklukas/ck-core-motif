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

start = text.rfind("CvtPixelToRenditionPixel(")
body = text[start : start + 1400]
if "from_val->size < sizeof(pixel)" not in body:
    raise SystemExit("Pixel converter does not reject undersized source values")
if "memcpy(&pixel, from_val->addr, sizeof(pixel));" not in body:
    raise SystemExit("Pixel converter does not copy a complete Pixel value")
if "*(int*)from_val->addr" in body:
    raise SystemExit("Pixel converter still truncates through int")

print("pixel converter input guards present")
