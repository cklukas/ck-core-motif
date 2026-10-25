#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"MainW.c").read_text()
assert text.count("if (w == NULL)") >= 4
for name in ("XmMainWindowSetAreas", "XmMainWindowSep1", "XmMainWindowSep2", "XmMainWindowSep3"):
    assert name in text
print("XmMainWindow APIs guard null widgets")
