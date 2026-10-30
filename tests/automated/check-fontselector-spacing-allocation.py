#!/usr/bin/env python3
"""Check FontSelector spacing control allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateSpacingBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateFontChoiceBox", start)]

assert "if (margs == NULL)" in body
assert "if (box == NULL)" in body
assert body.count("if (button == NULL)") >= 3
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback, ToggleProportional")

print("FontSelector spacing allocation is guarded")
