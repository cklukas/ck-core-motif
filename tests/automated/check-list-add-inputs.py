#!/usr/bin/env python3
"""Check that List add APIs reject invalid public inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nAPIAddItems")
end = source.index("XmListAddItems(Widget w", start)
region = source[start:end]
assert "item_count <= 0" in region
assert "items[i] == NULL" in region
for function, item_guard in (("XmListAddItem(", True), ("XmListAddItems(", False),
                             ("XmListAddItemsUnselected(", False),
                             ("XmListAddItemUnselected(", True)):
    start = source.index(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL" in body
    if item_guard:
        assert "item == NULL" in body
print("PASS list add APIs validate widget, count, and item inputs")
