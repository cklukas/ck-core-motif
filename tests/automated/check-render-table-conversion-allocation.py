#!/usr/bin/env python3
"""Ensure render-table property conversion handles its initial allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtToProp(Widget widget")
end = source.index("\n/*", start + 1)
region = source[start:end]

assert "buffer = XtMalloc(allocated_size);" in region
assert "if (buffer == NULL)" in region
assert "_XmAppUnlock(app);" in region
print("PASS render-table property conversion handles initial allocation failure")
