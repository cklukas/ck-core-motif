#!/usr/bin/env python3
"""Check UIL database integer-table counts before allocation and rebasing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
section = source[source.index("Boolean db_read_int_and_shorts") : source.index("void db_open_file")]

assert "item_count, int_table_count = 0, int_table_bytes;" in section
assert "item_count * sizeof(*table) > (size_t)header->table_size" in section
assert "table[i].values_cnt < 0" in section
assert "int_table_count > (size_t)UINT_MAX / sizeof(unsigned short)" in section
assert "int_table_bytes = sizeof(*int_table) * int_table_count;" in section
assert "XtCalloc(1, (Cardinal)int_table_bytes)" in section
print("UIL database integer-table counts are checked before allocation")
