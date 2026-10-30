#!/usr/bin/env python3
"""Check TabStack drag callback validates entry state."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("DragCallback(widget")
end = source.index("\n}", start)
body = source[start:end]
assert "widget == NULL || client == NULL || cbdata == NULL" in body
assert "XtIsSubclass((Widget) client, xmTabStackWidgetClass)" in body
assert "info = (XmAnyCallbackStruct*)cbdata" in body

print("TabStack drag callback guards present")
