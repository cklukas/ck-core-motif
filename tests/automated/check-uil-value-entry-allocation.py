#!/usr/bin/env python3
"""Check UIL token value construction handles node and payload failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("*sem_create_value_entry( value, length, value_type )")
end = source.index("\n\f\n/*", start)
body = source[start:end]

assert "if (value_entry == NULL)" in body
assert "if (value_entry->value.c_value == NULL)" in body
assert "if (value_entry->value.xms_value == NULL)" in body
assert body.count("sem_free_node ((sym_entry_type *)value_entry);") >= 2
print("UIL token value allocations are guarded")
