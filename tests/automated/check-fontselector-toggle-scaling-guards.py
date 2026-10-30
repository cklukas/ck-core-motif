#!/usr/bin/env python3
"""Check FontSelector scaling callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleScaling(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleBold", start)]

assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "family_info == NULL" in body
assert "if (XmFontS_size_box(fsw) != NULL)" in body

print("FontSelector scaling callback inputs are guarded")
