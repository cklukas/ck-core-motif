#!/usr/bin/env python3
"""Check FontSelector spacing refresh state before family lookup."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateFixedProportional(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: UpdateFamilies", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "num_families != 0 &&" in body
assert "cf = XmFontS_font_info(fsw)->current_font;" in body
assert "XmFontS_monospace(fsw) != NULL" in body
assert "XmFontS_proportional(fsw) != NULL" in body
assert "XmFontS_any_spacing(fsw) != NULL" in body

print("FontSelector fixed/proportional refresh is guarded")
