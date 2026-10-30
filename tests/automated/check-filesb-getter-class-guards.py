#!/usr/bin/env python3
"""Check FileSelectionBox synthetic getters validate widget classes."""

from pathlib import Path


source = Path("lib/Xm/FileSB.c").read_text(encoding="utf-8")
names = (
    "FSBGetDirSpec(", "FSBGetDirectory(", "FSBGetNoMatchString(",
    "FSBGetPattern(", "FSBGetFilterLabelString(",
    "FSBGetDirListLabelString(", "FSBGetDirListItems(",
    "FSBGetDirListItemCount(", "FSBGetListItems(",
    "FSBGetListItemCount(", "FSBGetDirMask(",
)
for name in names:
    start = source.rindex(name)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "XtIsSubclass(fs, xmFileSelectionBoxWidgetClass)" in body

print("FileSelectionBox getter class guards present")
