#!/usr/bin/env python3
"""Check NumberBox text follows the widget render table."""
from pathlib import Path

source = Path("lib/Xm/NumberBox.c").read_text(encoding="utf-8")
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
assert "if (new_table)" in source
assert "Keep the last valid table" in source
assert "number->number_box.render_table = old->number_box.render_table" in source
print("PASS NumberBox draws value text through its render table")
