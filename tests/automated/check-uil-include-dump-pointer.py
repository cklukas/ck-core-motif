#!/usr/bin/env python3
"""Check UIL include-file debug dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_include_file (")
section = source[start : source.rindex("sym_dump_section", start)]

assert '"%p  include file' in section
assert "(void *)az_symbol_entry" in section
assert "(unsigned)az_symbol_entry" not in section
print("UIL include-file debug dumps preserve pointer widths")
