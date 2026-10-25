#!/usr/bin/env python3
"""Check render-table export rejects null inputs before dereferencing them."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtToProp(")
end = source.index("\n\ntypedef enum", start)
region = source[start:end]

assert "if (widget == NULL || table == NULL || prop_return == NULL)" in region
assert "return 0;" in region
print("PASS XmRenderTableCvtToProp validates widget, table, and output")
