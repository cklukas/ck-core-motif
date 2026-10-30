#!/usr/bin/env python3
"""Check FontSelector SetValues tolerates partial child construction."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("SetValues(Widget old")
body = source[start:source.index("/*\tFunction Name: GetValuesHook", start)]

assert "XmFontS_show_font_toggle(set_fsw) != NULL" in body
assert "XmFontS_use_scaling_toggle(set_fsw) != NULL" in body
assert "if (option_menu != NULL)" in body
assert "XmFontS_text(set_fsw) != NULL" in body
assert "#define SetLabelIfPresent" in body
assert "SetLabelIfPresent(XmFontS_bold_toggle(set_fsw)" in body
assert "SetLabelIfPresent(XmFontS_text(set_fsw)" in body

print("FontSelector SetValues child updates are null-safe")
