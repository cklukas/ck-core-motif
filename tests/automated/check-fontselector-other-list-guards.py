#!/usr/bin/env python3
"""Check FontSelector Other-list construction guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("SetOtherList(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateChildren", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "XmFontS_family_box(fsw) == NULL" in body
assert "num > 0 && others == NULL" in body
assert "curFont == NULL" in body
assert "if (item != NULL)" in body

print("FontSelector Other-list construction is guarded")
