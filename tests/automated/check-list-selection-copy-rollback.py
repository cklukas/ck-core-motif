#!/usr/bin/env python3
"""Check List selection snapshots roll back partial XmString copies."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.rindex("BuildSelectedList(XmListWidget lw")
end = source.index("BuildSelectedPositions(", start)
region = source[start:end]

assert "if (lw->list.selectedItems[j] == NULL)" in region
assert "while (j != 0)" in region
assert "XmStringFree(lw->list.selectedItems[--j])" in region
assert "XtFree((char *)lw->list.selectedItems)" in region
assert "lw->list.selectedItems = NULL" in region
assert "lw->list.selectedItemCount = 0" in region
assert region.index("if (lw->list.selectedItems[j] == NULL)") < region.rindex("j++")
print("List selection snapshots roll back partial copies")
