#!/usr/bin/env python3
"""Ensure conversion contexts are initialized before publication."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static ConvertContext\nLookupContextBlock(")
body = source[start:source.index("static void\nClearContextBlock", start)]
assert "if (new_k == NULL)" in body
assert "if (cc == NULL)" in body
assert "cc->drag_context = (Widget)NULL;" in body
assert "cc->location_data = NULL;" in body
print("PASS transfer conversion contexts are initialized before publication")
