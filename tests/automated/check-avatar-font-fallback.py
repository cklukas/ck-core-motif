#!/usr/bin/env python3
"""Avatar uses the owned render table for both text measurement and drawing."""
from pathlib import Path
source = Path("lib/Xm/Avatar.c").read_text(encoding="utf-8")
assert "XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST)" in source
assert "XmStringExtent(avatar->avatar.font" in source
assert "XmStringDraw(display, text, avatar->avatar.font" in source
assert "XLoadQueryFont" not in source
assert "XmRenderTableFree(avatar->avatar.font)" in source
print("PASS Avatar render-table fallback and ownership")
