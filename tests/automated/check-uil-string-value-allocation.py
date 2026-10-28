#!/usr/bin/env python3
"""Check UIL string-value construction handles both allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLexAna.c").read_text()
start = source.index("sym_value_entry_type *create_str_entry")
body = source[start:]

assert "if (node == NULL)" in body
assert "node->value.c_value = XtCalloc(1, l_size + 1);" in body
assert "if (node->value.c_value == NULL)" in body
assert "sem_free_node ((sym_entry_type *)node);" in body
assert body.index("if (node == NULL)") < body.index("node->value.c_value = XtCalloc")
assert body.index("if (node->value.c_value == NULL)") < body.index("node->header.az_src_rec")
print("UIL string-value allocations are checked and cleaned up")
