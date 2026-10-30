#!/usr/bin/env python3
"""Check FontSelector size DropDown allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateSizesBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateBoldItalicBox", start)]

assert "if (margs == NULL)" in body
assert "if (c_box == NULL)" in body
assert body.index("if (c_box == NULL)") < body.index(
    "AddToXlfdOnlyList(fsw, c_box)")
assert body.index("XtFree((XtPointer) margs)") < body.index(
    "if (c_box == NULL)")

print("FontSelector size DropDown allocation is guarded")
