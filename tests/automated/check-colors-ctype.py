#!/usr/bin/env python3
"""Check defined ctype inputs during color-name normalization."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

for name in ("isascii", "isspace", "tolower", "toupper", "isdigit"):
    if f"{name}((unsigned char)" not in text:
        raise SystemExit(f"color selector ctype call is not unsigned: {name}")

print("color-name ctype conversion is defined for all bytes")
