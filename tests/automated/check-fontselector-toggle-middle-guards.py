#!/usr/bin/env python3
"""Check FontSelector middle-pane callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ToggleMiddlePane(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleNameWindow", start)]

assert "fsw == NULL" in body
assert "info == NULL" in body
assert "XmFontS_middle_pane(fsw) == NULL" in body
assert body.index("if (fsw == NULL") < body.index("if (info->set)")

print("FontSelector middle-pane callback inputs are guarded")
