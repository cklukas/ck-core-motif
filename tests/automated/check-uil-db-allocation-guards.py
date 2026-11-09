#!/usr/bin/env python3
"""Check UIL database loaders reject failed nonempty allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text(
    encoding="utf-8"
)

assert "if (string_size != 0 && string_table == NULL)" in source
assert "if (ptr == NULL)" in source
assert "if (table_bytes != 0 && table == NULL)" in source
assert "if (lengths == NULL)" in source
assert "XtFree ((char *)lengths);" in source
assert "if (table == NULL)" in source
assert "int_table_bytes = sizeof(*int_table) * int_table_count;" in source
assert "if (int_table_bytes != 0 && int_table == NULL)" in source
assert "db_read_table(int_table,\n\t\t\t   int_table_bytes)" in source

print("PASS UIL database loaders guard failed nonempty allocations")
