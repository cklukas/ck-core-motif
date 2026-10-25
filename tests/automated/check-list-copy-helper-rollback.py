#!/usr/bin/env python3
"""Check XmList copy helpers roll back partial copies and bound sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

def region(name, marker):
    start = source.index("\n" + name + "(XmListWidget", source.index(marker))
    return source[start:source.index("\n}\n", start) + 2]

items = region("CopyItems", "CopyItems - Copy")
selected = region("CopySelectedItems", "CopySelectedItems - Copy")
positions = region("CopySelectedPositions", "CopySelectedPositions - Copy")

for body, field, count in (
    (items, "lw->list.items = NULL;", "lw->list.itemCount = 0;"),
    (selected, "lw->list.selectedItems = NULL;", "lw->list.selectedItemCount = 0;"),
):
    assert "if (il[i] == NULL)" in body or "if (sl[i] == NULL)" in body
    assert "while (i != 0)" in body
    assert "XmStringFree(" in body
    assert field in body
    assert count in body

assert "UINT_MAX / sizeof(int)" in positions
assert "XtMalloc((Cardinal)size)" in positions
print("XmList copy helpers roll back partial copies and bound Xt sizes")
