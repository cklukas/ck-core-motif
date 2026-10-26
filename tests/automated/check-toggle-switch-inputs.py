#!/usr/bin/env python3
"""Check ToggleSwitch state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text(encoding="utf-8")
get_state = source[source.index("Boolean XmToggleSwitchGetState") : source.index("void XmToggleSwitchSetState")]
set_state = source[source.index("void XmToggleSwitchSetState") :]
assert "widget == NULL || !XmIsToggleSwitch(widget)" in get_state
assert "widget == NULL || !XmIsToggleSwitch(widget)" in set_state
print("PASS ToggleSwitch state APIs validate null widgets")
