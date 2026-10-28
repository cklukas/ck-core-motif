#!/usr/bin/env python3
"""Check the lazy UIL standard color table handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("static sym_value_entry_type\t*standard_color_table()")
body = source[start:]

assert "if (color_table == NULL)" in body
assert "if (color_table->value.z_color == NULL)" in body
assert "sem_free_node ((sym_entry_type *)color_table);" in body
assert "return sym_az_error_value_entry;" in body
print("UIL standard color-table allocation is guarded")
