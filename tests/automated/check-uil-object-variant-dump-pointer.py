#!/usr/bin/env python3
"""Check UIL object-variant debug dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_object_variant")
section = source[start : source.rindex("sym_dump_root_entry", start)]

assert '"%p  default obj var  next: %p' in section
assert "(void *)az_symbol_entry" in section
assert "(void *)az_symbol_entry->next" in section
assert "(unsigned)az_symbol_entry" not in section
print("UIL object-variant debug dumps preserve pointer widths")
