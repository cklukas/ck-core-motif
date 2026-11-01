#!/usr/bin/env python3
"""NumberBox relies on native text/arrow drawing, without duplicate GC state."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/NumberBox.c").read_text()
assert "xmTextFieldWidgetClass" in source and "xmArrowButtonWidgetClass" in source
assert "XCreateGC" not in source and "XFillPolygon" not in source
assert "XtRemoveWorkProc" in source
print("PASS NumberBox delegates drawing and cancels deferred layout")
