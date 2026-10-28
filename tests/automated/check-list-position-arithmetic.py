#!/usr/bin/env python3
"""Check List add/delete position arithmetic is validated before offsetting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
add_start = source.index("static void\nAPIAddItems")
add_end = source.index("void\nXmListAddItems", add_start)
add_body = source[add_start:add_end]
delete_start = source.index("void\nXmListDeleteItemsPos")
delete_end = source.index("int\nXmListGetKbdItemPos", delete_start)
delete_body = source[delete_start:delete_end]

assert "if (pos <= 0 || pos > lw->list.itemCount)" in add_body
assert "intern_pos = pos - 1;" in add_body
assert "if (pos <= 0)" in delete_body
assert "item_pos = pos - 1;" in delete_body
assert "item_count >= lw->list.itemCount - item_pos" in delete_body
assert "item_pos + item_count" not in delete_body
print("List add/delete position arithmetic is bounded")
