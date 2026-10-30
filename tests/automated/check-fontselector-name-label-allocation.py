#!/usr/bin/env python3
"""Check FontSelector name-label construction before use."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateChildren(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateTopWidgets", start)]

assert "temp = XtCreateWidget(\"nameLabel\"" in body
assert "if (temp == NULL)" in body
assert body.index("if (temp == NULL)") < body.index(
    "if (XmFontS_show_font_name(fsw))")
assert body.index("if (XmFontS_show_font_name(fsw))") < body.index(
    "XmFontS_name_label(fsw) = temp")

print("FontSelector name-label allocation is guarded")
