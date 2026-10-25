#!/usr/bin/env python3
"""Check Mwm graphics includes the limits used by its rectangle guard."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmGraphics.c"
text = source.read_text(encoding="utf-8")
assert "#include <limits.h>" in text
assert "amt > INT_MAX" in text
assert "UINT_MAX / sizeof(XRectangle)" in text
print("WmGraphics size-limit header present")
