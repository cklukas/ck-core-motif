#!/usr/bin/env python3
"""Check FontSelector encoding-menu allocation failure guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CreateEncodingMenu(XmFontSelectorWidget fsw")
body = source[start:source.index("/*\tFunction Name:", start + 1)]

assert body.count("if (margs == NULL)") >= 4
assert "if (menuShell == NULL)" in body
assert "if (pulldownMenu == NULL)" in body
assert "if (button == NULL)" in body
assert "XtDestroyWidget(menuShell)" in body

print("FontSelector encoding-menu allocation failures are guarded")
