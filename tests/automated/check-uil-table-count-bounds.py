#!/usr/bin/env python3
"""Check UIL table counts cannot wrap their 8-bit value-entry field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("value_entry->b_table_count = (unsigned char)count;")
body = source[source.rfind("for (table_entry", 0, start):source.index("\n\tbreak;", start)]

assert "if (count > UCHAR_MAX)" in body
assert "XtError(\"UIL table has too many entries\")" in body
assert "value_entry->b_table_count = (unsigned char)count;" in body
print("UIL table counts are bounded before narrowing")
