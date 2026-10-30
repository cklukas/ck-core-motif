#!/usr/bin/env python3
"""Check FileSelectionBox synthetic getters validate widgets and children."""

from pathlib import Path


source = Path("lib/Xm/FileSB.c").read_text()
for function in (
    "FSBGetDirectory",
    "FSBGetNoMatchString",
    "FSBGetPattern",
    "FSBGetFilterLabelString",
    "FSBGetDirListLabelString",
    "FSBGetDirListItems",
    "FSBGetDirListItemCount",
    "FSBGetListItems",
    "FSBGetListItemCount",
    "FSBGetDirMask",
):
    start = source.rindex(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "value == NULL" in body, function

assert "FS_FilterLabel(fs) == NULL" in source
assert "FS_DirList(fs) == NULL" in source
assert "SB_List(fs) != NULL" in source
assert "FS_FilterText(fs) == NULL" in source
print("FileSelectionBox synthetic getters validate state")
