#!/usr/bin/env python3
"""Check the UIL unknown-entry diagnostic preserves its pointer width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.index('"%p  unknown type:')
section = source[start : source.index("l_array =", start)]

assert '"%p  unknown type: %d  size: %d  byte: %x\\n"' in section
assert "(void *)az_symbol_entry" in section
print("UIL unknown-entry dumps preserve pointer widths")
