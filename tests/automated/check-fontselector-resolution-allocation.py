#!/usr/bin/env python3
"""Check FontSelector resolution control allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateResolutionBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateOtherChoiceBox", start)]

assert "if (margs == NULL)" in body
assert "if (box == NULL)" in body
assert body.count("if (button == NULL)") >= 3
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback, Toggle75DPI")

print("FontSelector resolution allocation is guarded")
