#!/usr/bin/env python3
"""Check FontSelector name-window callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleNameWindow(Widget w")
body = source[start:source.index("/*\tFunction Name: Toggle75DPI", start)]

assert "fsw == NULL || info == NULL" in body
assert "XmFontS_name_label(fsw) != NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "XmFontS_family_box(fsw) == NULL" in body
assert "if (str != NULL)" in body

print("FontSelector name-window callback inputs are guarded")
