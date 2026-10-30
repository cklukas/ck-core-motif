#!/usr/bin/env python3
"""Check TabStack drag action initialization and entry guards."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("XmTabBoxDragTab(widget")
end = source.index("\n}", start)
body = source[start:end]
assert "if (widget == NULL || event == NULL)" in body
assert "!XtIsSubclass((Widget) tab, xmTabStackWidgetClass)" in body
assert "event->xany.type != ButtonPress" in body
assert "child = IndexToTab(tab," in body
assert "if (child == NULL)" in body
assert "XmNclientData, child" in body

print("TabStack drag action guards and child initialization present")
