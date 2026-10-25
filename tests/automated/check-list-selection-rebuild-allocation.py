#!/usr/bin/env python3
"""Check List selection rebuilds handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

items_start = source.rindex("BuildSelectedList(XmListWidget lw")
items_end = source.index("BuildSelectedPositions(", items_start)
items = source[items_start:items_end]
assert "lw->list.selectedItems = (XmString *)XtMalloc" in items
assert "UINT_MAX / sizeof(XmString)" in items
assert "if (lw->list.selectedItems == NULL)" in items
assert items.index("if (lw->list.selectedItems == NULL)") < items.index("lw->list.selectedItems[j]")

positions_start = source.rindex("BuildSelectedPositions(XmListWidget lw")
positions_end = source.index("UpdateSelectedList(", positions_start)
positions = source[positions_start:positions_end]
assert "lw->list.selectedPositions = (int *) XtMalloc" in positions
assert "if (lw->list.selectedPositions == NULL)" in positions
assert positions.index("if (lw->list.selectedPositions == NULL)") < positions.index("lw->list.selectedPositions[nsel]")
print("PASS List selection rebuild allocation failures are handled")
