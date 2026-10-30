#!/usr/bin/env python3
"""Check FontSelector 100-DPI callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("Toggle100DPI(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleBothDPI", start)]

assert "fsw == NULL" in body
assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert body.index("if (fsw == NULL") < body.index("UnsetSiblings(w)")

print("FontSelector 100-DPI callback inputs are guarded")
