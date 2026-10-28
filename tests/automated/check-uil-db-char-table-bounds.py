#!/usr/bin/env python3
"""Check UIL database char-table products before allocation and pointer setup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
section = source[source.index("void db_read_char_table") : source.index("void db_read_length_and_string")]

assert "item_count = (size_t)header->num_items + 1;" in section
assert "item_count * sizeof(*ptr) > (size_t)header->table_size" in section
assert "header->num_items > SIZE_MAX / (size_t)num_bits" in section
assert "table_bytes = (size_t)header->num_items * (size_t)num_bits;" in section
assert "XtMalloc ((Cardinal)table_bytes)" in section
print("UIL database char-table products are checked before allocation")
