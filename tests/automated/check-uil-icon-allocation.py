#!/usr/bin/env python3
"""Check UIL icon construction handles node and payload allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("void\tsar_make_icon(")
end = source.index("static sym_value_entry_type\t*standard_color_table()", start)
body = source[start:end]

assert "if (icon_entry == NULL)" in body
assert "if (icon_entry->value.z_icon == NULL)" in body
assert "sem_free_node ((sym_entry_type *)icon_entry);" in body
assert "icon_entry = sym_az_error_value_entry;" in body
assert body.index("if (icon_entry == NULL)") < body.index("icon_entry->b_type = sym_k_icon_value;")
print("UIL icon node and payload allocations are guarded")
