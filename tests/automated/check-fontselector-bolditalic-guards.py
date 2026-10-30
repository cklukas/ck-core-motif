#!/usr/bin/env python3
"""Check FontSelector bold/italic refresh state and child guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateBoldItalic(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name:  UpdateFixedProportional", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "XmFontS_bold_toggle(fsw) == NULL" in body
assert "XmFontS_italic_toggle(fsw) == NULL" in body
assert "num_families != 0 &&" in body
assert "cf = XmFontS_font_info(fsw)->current_font;" in body

print("FontSelector bold/italic refresh is guarded")
