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
accelerator = text[text.index("tmpstr = XmStringCreateLocalized(menuItem->accelText);"):]
accelerator = accelerator[:accelerator.index("if ((accelKeySpec")]
assert "if (tmpstr != NULL)" in accelerator
assert "XmNacceleratorText" in accelerator
rename = text[text.index("labelString = XmStringGenerate"):text.index("case REMOVE", text.index("labelString = XmStringGenerate"))]
assert "if (labelString != NULL)" in rename
print("MWM menu label conversion is guarded")
