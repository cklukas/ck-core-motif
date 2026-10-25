#!/usr/bin/env python3
"""Check MWM menu label conversion is conditional and cleaned up."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmMenu.c"
text = source.read_text(encoding="utf-8")

needle = "tmpstr = XmStringCreateLocalized(menuItem->label);"
start = text.index(needle)
body = text[start:text.index("            }", start) + len("            }")]
assert "if (tmpstr != NULL)" in body
assert "XmNlabelString" in body
assert "XmStringFree(tmpstr);" in body
print("MWM menu label conversion is guarded")
