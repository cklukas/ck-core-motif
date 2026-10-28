#!/usr/bin/env python3
"""Check UIL procedure-reference dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_proc_ref")
section = source[start : source.index("sym_dump_forward_ref", start)]

assert '"%p proc ref  size: %d  proc def: %p  value: %p' in section
assert "(void *)az_proc_ref_entry" in section
assert "(void *)az_proc_ref_entry->az_proc_def" in section
assert "(void *)az_proc_ref_entry->az_arg_value" in section
print("UIL procedure-reference dumps preserve pointer widths")
