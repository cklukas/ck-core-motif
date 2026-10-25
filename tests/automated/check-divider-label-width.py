#!/usr/bin/env python3
"""Check Divider label width avoids Dimension overflow."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Divider.c"
text = source.read_text(encoding="utf-8")

start = text.index("static Dimension label_width")
body = text[start:text.index("static void Initialize", start)]
assert "size_t length" in body
assert "unsigned long pixels" in body
assert "(unsigned long)(Dimension)-1" in body
assert "Dimension width = (Dimension)pixels;" in body
print("Divider label width is bounded")
