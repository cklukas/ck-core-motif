#!/usr/bin/env python3
"""Check FontSelector size-change callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("SizeChanged(Widget w")
body = source[start:source.index("/*\tFunction Name: ChangeEncoding", start)]

assert "fsw == NULL || w == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "if (str == NULL)" in body
assert body.index("if (str == NULL)") < body.index("strtol(str")

print("FontSelector size-change callback inputs are guarded")
