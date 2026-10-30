#!/usr/bin/env python3
"""Check FontSelector font-choice control allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateFontChoiceBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateResolution Box", start)]

assert body.count("if (margs == NULL)") >= 2
assert "if (box == NULL)" in body
assert body.count("if (button == NULL)") >= 2
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback, XlfdMode")

print("FontSelector font-choice allocation is guarded")
