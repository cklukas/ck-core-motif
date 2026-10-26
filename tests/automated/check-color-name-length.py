#!/usr/bin/env python3
"""Check bounded color-name truncation and loop length accounting."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

if "if (len >= XmColorSelector_COLOR_NAME_SIZE)" not in text:
    raise SystemExit("color-name boundary check is missing")
if "len = XmColorSelector_COLOR_NAME_SIZE - 1;" not in text:
    raise SystemExit("color-name loop length is not reduced after truncation")
if '"%.255s", buf' not in text or '"%.255s", color_name' not in text:
    raise SystemExit("long-name warning arguments are not bounded")

print("color-name truncation keeps normalization loops bounded")
