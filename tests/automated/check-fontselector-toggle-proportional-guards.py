#!/usr/bin/env python3
"""Check FontSelector proportional callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleProportional(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleFixed", start)]

assert "fsw == NULL" in body
assert "info == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert body.index("if (fsw == NULL") < body.index("UnsetSiblings(w)")

print("FontSelector proportional callback inputs are guarded")
