#!/usr/bin/env python3
"""Ensure render-table import tests the parsed integer sentinel values."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtFromProp")
end = source.index("\nvoid\nXmRenderTableGetDefaultFontExtents", start)
region = source[start:end]

assert region.count("if (token -> integer != -1)") >= 5
assert "if (token -> type != -1)" not in region
print("PASS render-table import checks integer AS_IS sentinels")
