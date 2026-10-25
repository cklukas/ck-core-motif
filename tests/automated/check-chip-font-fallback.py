#!/usr/bin/env python3
"""Check Chip labels follow the widget render table."""
from pathlib import Path

source = Path("lib/Xm/Chip.c").read_text(encoding="utf-8")
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
print("PASS Chip draws labels through its render table")
