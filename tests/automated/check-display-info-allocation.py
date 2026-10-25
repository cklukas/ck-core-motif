#!/usr/bin/env python3
"""Check that XmDisplay initialization guards its private state allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Display.c").read_text(encoding="utf-8")
start = source.index("xmDisplay->display.displayInfo = (XtPointer) XtNew(XmDisplayInfo);")
end = source.index("xmDisplay->display.displayHasShapeExtension", start)
region = source[start:end]
assert "if (xmDisplay->display.displayInfo == NULL)" in region
assert "return;" in region
assert "SashCursor" in region
print("PASS XmDisplay private-state allocation is guarded")
