#!/usr/bin/env python3
"""Check rectangle-count and allocation guards in legacy drawing helpers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c"
text = source.read_text(encoding="utf-8")
if text.count("size > INT_MAX / 4") < 2:
    raise SystemExit("legacy rectangle count guards are incomplete")
if text.count("SIZE_MAX / (sizeof(XRectangle) * 4)") < 2:
    raise SystemExit("legacy rectangle allocation guards are incomplete")

print("legacy rectangle allocation sizing is bounded")
