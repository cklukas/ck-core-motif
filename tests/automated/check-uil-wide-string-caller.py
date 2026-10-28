#!/usr/bin/env python3
"""Check UIL wide-string semantic actions handle constructor failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("void\tsar_make_wchar_str")
body = source[start:source.index("\f\n/*", start)]

assert "wchar_str_entry = (sym_value_entry_type *) sem_create_wchar_str();" in body
assert "if (wchar_str_entry == NULL)" in body
assert body.index("if (wchar_str_entry == NULL)") < body.index("wchar_str_entry->b_expr_opr")
print("UIL wide-string semantic action checks constructor failure")
