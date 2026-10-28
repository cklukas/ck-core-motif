#!/usr/bin/env python3
"""Check SelectionBox list initialization validates match positions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text()
start = source.index("_XmSelectionBoxCreateList")
end = source.index("XmSelectionBoxGetChild(", start)
body = source[start:end]

assert "pos_count && position != NULL" in body
assert "sel->selection_box.list_selected_item_position = position[0]" in body

print("SelectionBox list initialization guards missing match positions")
