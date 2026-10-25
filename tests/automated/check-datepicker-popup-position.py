#!/usr/bin/env python3
"""Check DatePicker popup positioning handles oversized screens."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DatePicker.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void position_popup")
body = text[start:text.index("static void calendar_changed", start)]
assert "popup_width >= (Dimension)WidthOfScreen(screen)" in body
assert "popup_height >= (Dimension)HeightOfScreen(screen)" in body
assert "x = 0;" in body and "y = 0;" in body
assert "else if ((int)x + (int)popup_width" in body
print("DatePicker oversized popup positioning is clamped")
