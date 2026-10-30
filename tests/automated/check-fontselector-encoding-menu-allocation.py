#!/usr/bin/env python3
"""Check FontSelector encoding-menu failure handling."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateEncodingMenu(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CheckEncoding", start)]

assert "if (omenu == NULL)" in body
assert body.index("if (omenu == NULL)") < body.index(
    "XtSetArg(largs[num_largs], XmNwidth")
assert "XmString label = XmStringCreateLocalized(*encodings);" in body
assert "if (label == NULL)" in body
assert body.index("if (label == NULL)") < body.index(
    "margs[button_label].value", body.index("if (label == NULL)"))

print("FontSelector encoding-menu allocation is guarded")
