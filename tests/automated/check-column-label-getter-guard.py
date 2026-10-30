#!/usr/bin/env python3
"""Check Column's synthetic label getter validates callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Column.c").read_text(encoding="utf-8")
start = source.rindex("Get_entryLabelString")
end = source.index("\n}", start)
body = source[start:end]
assert "widget == NULL || value == NULL" in body
assert "XtIsSubclass(widget, xmColumnWidgetClass)" in body
assert "*value = (XtArgVal) NULL" in body

print("Column label getter guard present")
