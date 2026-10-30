#!/usr/bin/env python3
"""Check FontSelector current-font retrieval after partial initialization."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("GetValuesHook(Widget w")
body = source[start:source.index("/************************************************************", start)]

assert "XmFontS_font_info(fsw) == NULL" in body
assert "XmFontS_get_font(fsw) == NULL" in body
assert "if (str_ptr == NULL)" in body
assert "*str_ptr = NULL;" in body
assert body.index("*str_ptr = NULL;") < body.index(
    "BuildFontString(fsw, cf")

print("FontSelector current-font retrieval is partial-init safe")
