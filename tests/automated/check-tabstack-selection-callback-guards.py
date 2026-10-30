#!/usr/bin/env python3
"""Check TabStack selection callback validates callback and active state."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("TabSelectedCallback(Widget widget")
end = source.index("\n}", start)
body = source[start:end]
assert "widget == NULL || cbdata == NULL" in body
assert "!XtIsSubclass((Widget) tab, xmTabStackWidgetClass)" in body
assert "old == NULL" in body

print("TabStack selection callback guards present")
