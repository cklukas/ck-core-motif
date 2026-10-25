#!/usr/bin/env python3
"""Check DrawUtils includes the limit definitions used by its guard."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "DrawUtils.c"
text = source.read_text(encoding="utf-8")
assert "#include <limits.h>" in text
assert "size > UINT_MAX / sizeof(XRectangle)" in text
print("DrawUtils size-limit header present")
