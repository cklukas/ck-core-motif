#!/usr/bin/env python3
"""Check UIL module-entry dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_module")
section = source[start : source.index("sym_dump_color_item", start)]

assert '"%p module size: %d  name: %p  version: %p' in section
assert "(void *)az_module_entry" in section
assert "(void *)az_module_entry->obj_header.az_name" in section
assert "(void *)az_module_entry->az_version" in section
print("UIL module-entry dumps preserve pointer widths")
