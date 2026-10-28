#!/usr/bin/env python3
"""Check UIL string-table conversion guards its temporary node."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()
start = source.index("cstr = (sym_value_entry_type *) sem_create_cstr();")
body = source[start:start + 360]

assert "if (cstr == NULL)" in body
assert body.index("if (cstr == NULL)") < body.index("sem_append_str_to_cstr")
assert body.index("if (cstr == NULL)") < body.index("_sym_copy_entry")
print("UIL string-table conversion checks its temporary compound node")
