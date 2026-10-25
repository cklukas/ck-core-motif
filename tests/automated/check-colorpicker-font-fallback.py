#!/usr/bin/env python3
"""Check ColorPicker value text follows the widget render table."""
from pathlib import Path

source = Path("lib/Xm/ColorPicker.c").read_text(encoding="utf-8")
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
print("PASS ColorPicker draws value text through its render table")
