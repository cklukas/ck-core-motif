#!/usr/bin/env python3
"""Check UIL value-entry dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_value")
section = source[start : source.index("sym_dump_source_info", start)]

assert '"%p value  size: %d  name: %p  %s%s%s%s"' in section
assert "(void *)az_value_entry" in section
assert "(void *)az_value_entry->obj_header.az_name" in section
print("UIL value-entry dumps preserve pointer widths")
