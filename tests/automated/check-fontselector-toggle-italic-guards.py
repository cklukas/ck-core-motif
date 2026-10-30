#!/usr/bin/env python3
"""Check FontSelector italic callback state and string guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleItalic(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleMiddlePane", start)]

assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "family_info == NULL" in body
assert "if (slant == NULL)" in body
assert "strncpy(cf->slant, slant, SLANT_LEN)" in body

print("FontSelector italic callback inputs are guarded")
