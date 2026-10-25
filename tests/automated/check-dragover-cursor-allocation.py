#!/usr/bin/env python3
"""Check drag cursor creation and cache allocation failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragOverS.c").read_text()
start = source.index("GetDragIconCursor(")
end = source.index("\n/************************************************************************\n *\n *  Initialize", start)
region = source[start:end]

assert "if (cursor == None)" in region
assert "cursorCache = XtNew(XmDragCursorRec);" in region
assert "if (cursorCache == NULL)" in region
assert "XFreeCursor(display, cursor);" in region
allocation = region.index("cursorCache = XtNew(XmDragCursorRec);")
assert region.index("if (cursorCache == NULL)", allocation) < region.index("cursorCache->sourceIcon", allocation)
print("PASS drag cursor allocation failure is handled")
