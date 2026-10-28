#!/usr/bin/env python3
"""Check UIL section debug dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_section (")
section = source[start : source.rindex("sym_dump_object_variant")]

assert '"%p  %s section' in section
assert "(void *)az_symbol_entry->prev_section" in section
assert "(void *)az_symbol_entry->next" in section
assert "(void *)az_symbol_entry->entries" in section
assert "(unsigned)az_symbol_entry" not in section
print("UIL section debug dumps preserve pointer widths")
