#!/usr/bin/env python3
"""Check FontSelector secondary-choice allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateOtherChoiceBox(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateEncodingMenu", start)]

assert body.count("if (margs == NULL)") >= 2
assert "if (box == NULL)" in body
assert body.count("if (button == NULL)") >= 2
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback")
assert "if (XmFontS_option_menu(fsw) != NULL)" in body

print("FontSelector secondary-choice allocation is guarded")
