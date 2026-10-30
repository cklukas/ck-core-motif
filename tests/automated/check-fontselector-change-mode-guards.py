#!/usr/bin/env python3
"""Check FontSelector mode-transition boundary guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ChangeMode(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: SetOtherList", start)]

assert "fsw == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "XmFontS_family_box(fsw) == NULL" in body
assert "i < num && widgets != NULL" in body
assert "if (*widgets != NULL)" in body
assert "if (mode_toggle != NULL)" in body
assert "if (temp == NULL)" in body

print("FontSelector ChangeMode boundaries are guarded")
