#!/usr/bin/env python3
"""Check UIL color-item dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_color_item")
section = source[start : source.index("sym_dump_parent_list_item", start)]

assert '"%p color_item  size: %d  letter: %c  index: %d  color: %p  next: %p' in section
assert "(void *)az_color_item_entry" in section
assert "(void *)az_color_item_entry->az_color" in section
assert "(void *)az_color_item_entry->az_next" in section
print("UIL color-item dumps preserve pointer widths")
