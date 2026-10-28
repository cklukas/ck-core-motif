#!/usr/bin/env python3
"""Check XmDragCancel rejects non-drag widgets before casting."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("void \nXmDragCancel(")
end = source.index("/*ARGSUSED*/", start)
body = source[start:end]

assert "XtIsSubclass(dragContext, xmDragContextClass)" in body
assert body.index("XtIsSubclass") < body.index("DragCancel((XmDragContext)")

print("PASS XmDragCancel validates the widget class")
