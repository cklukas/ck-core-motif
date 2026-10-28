#!/usr/bin/env python3
"""Check UIL root metadata copies cannot overflow fixed symbol fields."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarMod.c").read_text()
start = source.index("static void\nsar_copy_fixed_string")
root = source.index("void\tsar_create_root", start)
body = source[start:root]
creation = source[root:source.index("\n}\n", root)]

assert "if (length >= destination_size)" in body
assert "destination[length] = '\\0';" in body
assert "strcpy (sym_az_root_entry->file_name" not in creation
assert creation.count("sar_copy_fixed_string(") == 2
print("UIL root filename metadata is bounded")
