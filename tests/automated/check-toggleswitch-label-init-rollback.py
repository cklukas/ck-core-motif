#!/usr/bin/env python3
"""Check ToggleSwitch initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text()
start = source.index("static void Initialize(Widget request")
end = source.index("\nstatic void Destroy", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = XmStringCopy(sw->toggle_switch.label);" in region
assert "if (replacement != NULL)" in region
assert "sw->toggle_switch.label = XmStringCreateLocalized(\"\");" in region
assert "if (sw->toggle_switch.font && sw->toggle_switch.label)" in region
print("ToggleSwitch initialization handles label allocation failure")
