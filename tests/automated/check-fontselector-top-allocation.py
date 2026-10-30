#!/usr/bin/env python3
"""Check FontSelector top-pane and option-button allocation guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateTopWidgets(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: CreateMiddleArea", start)]

assert body.count("if (margs == NULL)") >= 2
assert "if (pane == NULL)" in body
assert "if (button == NULL)" in body
assert body.index("if (button == NULL)") < body.index(
    "XtAddCallback(button, XmNvalueChangedCallback")

print("FontSelector top-pane allocation is guarded")
