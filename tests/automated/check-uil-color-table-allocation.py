#!/usr/bin/env python3
"""Check UIL color-table construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("void\tsar_make_color_table")
end = source.index("\n\f\n/*", start)
body = source[start:end]

assert "if (color_table_entry == NULL)" in body
assert "if (color_table_entry->value.z_color == NULL)" in body
assert "sem_free_node ((sym_entry_type *)color_table_entry);" in body
assert "color_table_entry = sym_az_error_value_entry;" in body
print("UIL color-table allocations are guarded")
