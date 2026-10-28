#!/usr/bin/env python3
"""Check UIL compound-string construction propagates allocation failures."""
from pathlib import Path

lex = (Path(__file__).parents[2] / "clients" / "uil" / "UilLexAna.c").read_text()
comp = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarComp.c").read_text()

mixed = lex[lex.index("cstr_entry = sem_create_cstr();"):lex.index("  case token_lstr:", lex.index("cstr_entry = sem_create_cstr();"))]
localized = lex[lex.index("case token_lstr:"):]
create = comp[comp.index("sym_value_entry_type \t*sem_create_cstr()"):comp.index("\n}\n\f\n/*", comp.index("sym_value_entry_type \t*sem_create_cstr()"))]

assert "if (cstr_entry == NULL)" in mixed
assert mixed.count("if (str_entry == NULL)") >= 2
assert "if (l_length > INT_MAX - (l_max_lex_buffer_pos + 1))" in localized
assert "if (str_entry == NULL)" in localized
assert "if (az_cstr_entry == NULL)" in create
print("UIL compound-string allocation failures are propagated")
