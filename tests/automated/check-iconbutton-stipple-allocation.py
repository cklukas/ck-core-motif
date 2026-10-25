#!/usr/bin/env python3
"""Check grey-stipple cache allocation failure is handled."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text()
start = source.index("GetGreyStipple(")
start = source.index("set = (StippleInfo *) XtMalloc", start)
body = source[start:source.index("set->stipple", start)]
assert "if (set == NULL)" in body
assert "return (Pixmap)None;" in body
print("PASS IconButton stipple allocation is guarded")
