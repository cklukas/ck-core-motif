#!/usr/bin/env python3
"""Check MultiList APIs validate widget class before internal-child access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text(encoding="utf-8")
names = (
    "XmMultiListGetSelectedRows(", "XmMultiListUnselectAllItems(",
    "XmMultiListUnselectItem(", "XmMultiListToggleRow(",
    "XmMultiListSelectItems(", "XmMultiListDeselectItems(",
    "XmMultiListSelectAllItems(", "XmMultiListSelectRow(",
    "XmMultiListDeselectRow(", "XmMultiListGetSelectedRowArray(",
    "XmMultiListMakeRowVisible(",
)
for name in names:
    start = source.index(name)
    region = source[start:source.index("\n}\n", start)]
    assert "XtIsSubclass(w, xmMultiListWidgetClass)" in region, name
    assert region.index("XtIsSubclass(w, xmMultiListWidgetClass)") < region.index("XmMultiList_ilist(elist)"), name
print("PASS MultiList APIs validate widget class before internal-child access")
