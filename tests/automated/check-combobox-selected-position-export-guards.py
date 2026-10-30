#!/usr/bin/env python3
"""Check ComboBox selected-position export tolerates pre-initialization."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
start = source.index("static void\nCBGetSelectedPos")
body = source[start:source.index("/*\n * CBGetColumns", start)]

assert "if (widget == NULL || value == NULL)" in body
assert "if (!cb->composite.num_children || CB_List(cb) == NULL)" in body
assert "XmNselectedPositions" in body
assert "*value = result;" in body

print("ComboBox selected-position export is lifecycle-safe")
