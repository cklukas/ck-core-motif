#!/usr/bin/env python3
"""Check FontSelector primary child construction guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateChildren(XmFontSelectorWidget fsw")
body = source[start:source.index("/*\tFunction Name: CreateTopWidgets", start)]

assert body.count("if (margs == NULL)") >= 3
assert "if (box == NULL)" in body
assert "if (temp == NULL)" in body
assert body.index("if (temp == NULL)") < body.index(
    "XtAddCallback(temp, XmNlosingFocusCallback")

print("FontSelector primary child construction is guarded")
