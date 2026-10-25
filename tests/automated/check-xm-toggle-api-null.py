#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ToggleB.c").read_text()
assert text.count("if (w == NULL)") >= 3
for name in ("XmToggleButtonGetState", "XmToggleButtonSetState", "XmToggleButtonSetValue"):
    assert name in text
print("XmToggleButton state APIs guard null widgets")
