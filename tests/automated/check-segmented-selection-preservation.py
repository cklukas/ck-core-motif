#!/usr/bin/env python3
"""Check SegmentedControl preserves selection by ID before freeing old items."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("Boolean XmSegmentedControlSetItems")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "const char *selected_id = NULL;" in body
assert "strcmp(copy[selected_index].id, selected_id) == 0" in body
assert "preserved_selected = (int)selected_index;" in body
assert body.index("strcmp(copy[selected_index].id, selected_id) == 0") < body.index(
    "XtFree((char *)old_items);")
assert "copy[selected_index].id == selected_id" not in body
print("PASS SegmentedControl preserves selection by ID before old-item teardown")
