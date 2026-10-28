#!/usr/bin/env python3
"""Check UIL database integer-table counts before allocation and rebasing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
section = source[source.index("void db_read_int_and_shorts") : source.index("void db_open_file")]

assert "item_count, int_table_count = 0;" in section
assert "item_count * sizeof(*table) > (size_t)header->table_size" in section
assert "table[i].values_cnt < 0" in section
assert "int_table_count > (size_t)UINT_MAX / sizeof(unsigned short)" in section
assert "sizeof(unsigned short) * int_table_count" in section
print("UIL database integer-table counts are checked before allocation")
