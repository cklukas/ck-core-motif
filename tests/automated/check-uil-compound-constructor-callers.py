#!/usr/bin/env python3
"""Check UIL parser compound-string constructors handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
for marker in ("cstr_entry->b_expr_opr = sym_k_comp_str_op;",
               "/* Evaluate type. */"):
    position = source.index(marker)
    prefix = source[max(0, position - 180):position]
    assert "if (cstr_entry == NULL)" in prefix
print("UIL compound-string parser constructors check allocation")
