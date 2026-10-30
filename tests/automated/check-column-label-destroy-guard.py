#!/usr/bin/env python3
"""Check Column label teardown validates its client widget."""

from pathlib import Path


source = Path("lib/Xm/Column.c").read_text(encoding="utf-8")
start = source.rindex("XmColumnLabelDestroyedCallback(")
end = source.index("\n}", start)
body = source[start:end]
assert "field == NULL" in body
assert "XtIsSubclass(field, xmColumnWidgetClass)" in body
assert "XiC(field)->label_widget = NULL" in body

print("Column label-destroy guard present")
