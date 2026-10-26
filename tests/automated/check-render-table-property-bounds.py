#!/usr/bin/env python3
"""Check the render-table property length fits the parser's int offsets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtFromProp")
body = source[start:source.index("_XmAppLock(app);", start)]
assert "len > (unsigned int)INT_MAX" in body
assert "w == NULL || prop == NULL || len > (unsigned int)INT_MAX" in body
print("PASS render-table property length is representable")
