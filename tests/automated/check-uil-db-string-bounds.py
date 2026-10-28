#!/usr/bin/env python3
"""Check UIL database string-table accumulation and capacity bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
section = source[source.index("void db_read_ints_and_string") : source.index("void db_read_char_table")]

assert "size_t\t\tstring_size = 0;" in section
assert "(size_t)header->num_items >" in section
assert "(size_t)header->table_size / sizeof(*table)" in section
assert "string_size > (size_t)UINT_MAX" in section
assert "XtMalloc ((Cardinal)string_size)" in section
print("UIL database string-table sizes are checked before allocation and reads")
