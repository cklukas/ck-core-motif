#!/usr/bin/env python3
"""Check FontSelector family-list refresh state and failure guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateFamilies(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: UpdateSizes", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "XmFontS_family_box(fsw) == NULL" in body
assert "num < 0 || (num != 0 && fam == NULL)" in body
assert "if (temp == NULL)" in body

print("FontSelector family refresh is guarded")
