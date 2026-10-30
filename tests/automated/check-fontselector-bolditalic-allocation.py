#!/usr/bin/env python3
"""Check FontSelector bold/italic control allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateBoldItalicBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateSpacingBox", start)]

assert body.count("if (margs == NULL)") >= 2
assert body.count("if (button == NULL)") >= 2
assert "if (box == NULL)" in body
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback, ToggleBold")

print("FontSelector bold/italic allocation is guarded")
