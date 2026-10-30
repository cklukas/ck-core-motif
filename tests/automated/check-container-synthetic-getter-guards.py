#!/usr/bin/env python3
"""Check Container synthetic getters validate widget and output storage."""

from pathlib import Path


source = Path("lib/Xm/Container.c").read_text(encoding="utf-8")
names = ("GetDetailHeader(", "GetDetailHeaderCount(", "GetOutlineColumnWidth(")
for name in names:
    start = source.rindex(name)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL || value == NULL" in body
    assert "XtIsSubclass(wid, xmContainerWidgetClass)" in body
    assert "*value = (XtArgVal) 0" in body or "*value = (XtArgVal) NULL" in body

print("Container synthetic getter guards present")
