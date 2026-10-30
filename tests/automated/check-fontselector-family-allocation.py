#!/usr/bin/env python3
"""Check FontSelector family DropDown allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateFamilyBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateSizesBox", start)]

assert "if (margs == NULL)" in body
assert "if (c_box == NULL)" in body
assert body.index("if (c_box == NULL)") < body.index(
    "XtAddCallback(c_box, XmNverifyTextCallback")
assert body.index("XtFree((XtPointer) margs)") < body.index(
    "if (c_box == NULL)")

print("FontSelector family DropDown allocation is guarded")
