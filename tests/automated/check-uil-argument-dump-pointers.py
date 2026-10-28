#!/usr/bin/env python3
"""Check UIL argument dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_argument")
section = source[start : source.index("sym_dump_control", start)]

assert '"  arg name: %p  arg value: %p\\n"' in section
assert "(void *)az_argument_entry->az_arg_name" in section
assert "(void *)az_argument_entry->az_arg_value" in section
print("UIL argument dumps preserve pointer widths")
