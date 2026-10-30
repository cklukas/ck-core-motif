#!/usr/bin/env python3
"""Check FontSelector size-list refresh state and failure guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateSizes(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateChildren", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "XmFontS_size_box(fsw) == NULL" in body
assert "family_info == NULL" in body
assert "if (temp == NULL)" in body

print("FontSelector size refresh is guarded")
