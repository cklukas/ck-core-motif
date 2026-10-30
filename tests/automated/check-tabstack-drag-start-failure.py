#!/usr/bin/env python3
"""Check TabStack drag setup stops when XmDragStart fails."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("XmTabBoxDragTab(widget")
end = source.index("\n}", start)
body = source[start:end]
drag = body.index("drag = XmDragStart(parent, event, args, n)")
guard = body.index("if (drag == NULL)", drag)
callback = body.index("XtAddCallback(drag", drag)
assert guard < callback
assert "return;" in body[guard:callback]

print("TabStack drag-start failure guard present")
