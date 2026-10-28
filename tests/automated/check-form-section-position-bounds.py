#!/usr/bin/env python3
"""Check FormSection layout origins stay within Position."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FormSection.c").read_text()

assert "static Position position(long long value)" in source
assert "if (value < SHRT_MIN)" in source
assert "if (value > SHRT_MAX)" in source
layout = source[source.index("static void layout") : source.index("static void negotiate")]
assert "x = position(section->section.margin_width);" in layout
assert "position(section->section.margin_height)" in layout
assert "y = position((long long)section->section.margin_height" in layout
assert "y = position((long long)y + content_height" in layout
assert "y = (Position)(section->section.margin_height + hh" not in layout
print("PASS FormSection layout origins are bounded")
