#!/usr/bin/env python3
"""Check UIL control dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_control")
section = source[start : source.index("sym_dump_callback", start)]

assert '"  obj: %p\\n"' in section
assert "(void *)az_control_entry->az_con_obj" in section
print("UIL control dumps preserve pointer widths")
