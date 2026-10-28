#!/usr/bin/env python3
"""Check empty Lists cannot expose or select a phantom keyboard item."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
get_start = source.index("int\nXmListGetKbdItemPos")
get_end = source.index("Boolean\nXmListSetKbdItemPos", get_start)
get_body = source[get_start:get_end]
set_start = get_end
set_end = source.index("Boolean\nXmListGetMatchPos", set_start)
set_body = source[set_start:set_end]

assert "lw->list.items == NULL || lw->list.itemCount <= 0" in get_body
assert "lw->list.itemCount <= 0" in set_body
print("Empty List keyboard positions are rejected")
