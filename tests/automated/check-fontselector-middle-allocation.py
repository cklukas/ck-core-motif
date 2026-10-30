#!/usr/bin/env python3
"""Check FontSelector middle-pane allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateMiddleArea(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateFamilyBox", start)]

assert body.count("if (margs == NULL)") >= 2
assert "if (pane == NULL)" in body
assert "if (top_pane == NULL)" in body
assert body.index("if (top_pane == NULL)") < body.index(
    "CreateFontChoiceBox(fsw, top_pane")

print("FontSelector middle-pane allocation is guarded")
