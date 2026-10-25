#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Display.c").read_text()
assert text.count("if (w == NULL)") >= 2
assert "if (display == NULL)" in text
for name in ("XmGetDragContext", "_XmGetDragContextFromHandle", "XmGetXmDisplay"):
    assert name in text
print("Xm display and drag APIs validate null inputs")
