#!/usr/bin/env python3
"""Check UIL database name-length tables reject invalid totals."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
section = source[source.index("void db_read_length_and_string") : source.index("void db_read_int_and_shorts")]

assert "item_count = (size_t)header->num_items + 1;" in section
assert "item_count * sizeof(*table) > (size_t)header->table_size" in section
assert "lengths_bytes = item_count * sizeof(*lengths);" in section
assert "lengths[i] < 0" in section
assert "string_size > (size_t)UINT_MAX" in section
assert "XtMalloc ((Cardinal)lengths_bytes)" in section
print("UIL database name-length tables reject invalid counts and totals")
