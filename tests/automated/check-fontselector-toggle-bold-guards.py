#!/usr/bin/env python3
"""Check FontSelector bold callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleBold(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleItalic", start)]

assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "family_info == NULL" in body

print("FontSelector bold callback inputs are guarded")
