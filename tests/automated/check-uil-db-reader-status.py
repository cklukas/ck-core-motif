#!/usr/bin/env python3
"""Check that incomplete UIL database records stop the enclosing load."""

from pathlib import Path


root = Path(__file__).parents[2]
source = (root / "clients" / "uil" / "UilDB.c").read_text(encoding="utf-8")
header = (root / "clients" / "uil" / "UilDefI.h").read_text(encoding="utf-8")

reader = source[source.index("static Boolean\ndb_read_table") : source.index("void db_incorporate()")]
assert "if (table_size == 0)" in reader
assert "return True;" in reader
assert "return False;" in reader

main = source[source.index("void db_incorporate()") : source.index("db_incorporate_done:")]
for function in (
    "db_read_ints_and_string",
    "db_read_char_table",
    "db_read_length_and_string",
    "db_read_int_and_shorts",
):
    assert f"extern Boolean {function}" in header
    assert f"if (!{function}" in main
    assert "goto db_incorporate_done;" in main[main.index(f"if (!{function}") :]
    body_start = source.index(f"Boolean {function}")
    body_end = source.find("\n\f", body_start)
    body = source[body_start : body_end if body_end != -1 else None]
    assert "db_read_table" in body
    assert "return False;" in body
    assert "return True;" in body

print("UIL database readers report incomplete records to the enclosing loader")
