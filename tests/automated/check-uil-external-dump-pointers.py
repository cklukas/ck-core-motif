#!/usr/bin/env python3
"""Check UIL external-definition dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_external_def")
section = source[start : source.index("sym_dump_proc_def", start)]

assert '"%p external def  size: %d  next external: %p  object: %p' in section
assert "(void *)az_external_def_entry" in section
assert "(void *)az_external_def_entry->az_next_object" in section
assert "(void *)az_external_def_entry->az_name" in section
print("UIL external-definition dumps preserve pointer widths")
