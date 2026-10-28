#!/usr/bin/env python3
"""Check UIL name-entry dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_name")
section = source[start : source.index("sym_dump_module", start)]

assert '"%p name size: %d  next name: %p  object: %p"' in section
assert "(void *)az_name_entry" in section
assert "(void *)az_name_entry->az_next_name_entry" in section
assert "(void *)az_name_entry->az_object" in section
print("UIL name-entry dumps preserve pointer widths")
