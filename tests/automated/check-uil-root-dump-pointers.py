#!/usr/bin/env python3
"""Check UIL root-entry debug dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_root_entry")
section = source[start : source.rindex("char *", start)]

assert '"%p  root  tag: %d  module: %p  sections: %p' in section
assert "(void *)az_symbol_entry" in section
assert "(void *)az_symbol_entry->module_hdr" in section
assert "(void *)az_symbol_entry->sections" in section
assert "(unsigned)az_symbol_entry" not in section
print("UIL root-entry debug dumps preserve pointer widths")
