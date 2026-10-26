#!/usr/bin/env python3
"""Check XmDragCancel validates its drag-context widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text(encoding="utf-8")
start = source.index("XmDragCancel(")
region = source[start:]
assert "if (dragContext == NULL)" in region
assert region.index("if (dragContext == NULL)") < region.index("_XmWidgetToAppContext(dragContext)")
print("PASS XmDragCancel validates null drag contexts")
