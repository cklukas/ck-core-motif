#!/usr/bin/env python3
"""Check clipboard callback-table growth is bounded before allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("if (! found) {")
body = source[start:source.index("newCbProcTable =", start)]
assert "oldLimit > INT_MAX - 20" in body
assert "UINT_MAX / sizeof(XmCutPasteProc)" in body
assert "UINT_MAX / sizeof(long)" in body
print("PASS clipboard callback-table growth bounds")
