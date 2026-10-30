#!/usr/bin/env python3
"""Check FontSelector any-spacing callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleBothSpacing(Widget w")
body = source[start:source.index("/*\tFunction Name: XlfdMode", start)]

assert "fsw == NULL" in body
assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert body.index("if (fsw == NULL") < body.index("UnsetSiblings(w)")

print("FontSelector any-spacing callback inputs are guarded")
