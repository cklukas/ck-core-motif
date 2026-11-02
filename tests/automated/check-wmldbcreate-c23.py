#!/usr/bin/env python3
"""Check the Exm WML database generator uses typed C23 definitions."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "lib"
    / "Exm"
    / "wml"
    / "wmldbcreate.c"
).read_text(encoding="utf-8")

contracts = (
    "#include <stdlib.h>",
    "#include <string.h>",
    "main(int argc, char **argv)",
    "emit_header(_db_header_ptr header)",
    "emit_chars(int table_id)",
    "emit_ints_and_string(int table_id)",
    "emit_char_table(int table_id)",
    "emit_length_and_string(int table_id)",
    "emit_shorts(int table_id)",
    "emit_int_and_table_shorts(int table_id)",
    "emit_ints(int table_id)",
)
for contract in contracts:
    assert contract in source, f"missing typed wmldbcreate contract: {contract}"

legacy_definitions = (
    "int main(argc, argv)\nint argc;",
    "void emit_header(header)\n_db_header_ptr header;",
    "void emit_chars(table_id)\n    int",
    "void emit_char_table(table_id)\nint",
)
for legacy in legacy_definitions:
    assert legacy not in source, f"legacy K&R wmldbcreate definition remains: {legacy!r}"

print("Exm WML database generator uses typed C23 definitions")
