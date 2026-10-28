#!/usr/bin/env python3
"""Check UIL procedure-definition dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_proc_def")
section = source[start : source.index("sym_dump_proc_ref", start)]

assert '"%p proc def  size: %d  name: %p %s%s%s%s' in section
assert "(void *)az_proc_def_entry" in section
assert "(void *)az_proc_def_entry->obj_header.az_name" in section
print("UIL procedure-definition dumps preserve pointer widths")
