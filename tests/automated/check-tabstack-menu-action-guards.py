#!/usr/bin/env python3
"""Check TabStack menu action validates widget, event, and parent state."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("XmTabStackMenu(widget")
end = source.index("\n}", start)
body = source[start:end]
assert "if (widget == NULL || event == NULL)" in body
assert "if (parent == NULL)" in body
assert "!XtIsSubclass((Widget) tab, xmTabStackWidgetClass)" in body

print("TabStack menu action guards present")
