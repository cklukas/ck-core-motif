#!/usr/bin/env python3
"""Check List top-position import rejects invalid widgets and INT_MIN."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text(encoding="utf-8")
start = source.rindex("CvtToInternalPos(")
end = source.index("\n}", start)
body = source[start:end]
assert "wid == NULL || value == NULL" in body
assert "XtIsSubclass(wid, xmListWidgetClass)" in body
assert "*(int *) value == INT_MIN" in body
assert "(* (int *) value)--" in body

print("List top-position import guards present")
