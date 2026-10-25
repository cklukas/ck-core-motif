#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"XmExtUtil.c").read_text()
assert text.count("if (w == NULL)") >= 7
for name in ("_XmMoveWidget", "_XmResizeWidget", "_XmConfigureWidget"):
    assert name in text, name
print("Xm geometry wrappers guard null widgets")
