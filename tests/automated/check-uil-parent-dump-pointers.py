#!/usr/bin/env python3
"""Check UIL parent-list debug dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_parent_list_item")
section = source[start : source.rindex("sym_dump_external", start)]

assert '"%p parent_list  size: %d  parent: %p  next: %p' in section
assert "(void *)az_parent_list_item" in section
assert "(void *)az_parent_list_item->parent" in section
assert "(void *)az_parent_list_item->next" in section
print("UIL parent-list debug dumps preserve pointer widths")
