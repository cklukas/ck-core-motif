#!/usr/bin/env python3
"""Check raw UIL database tables do not fread through an unchecked pointer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text(
    encoding="utf-8"
)

helper = source[source.index("db_allocate_table(size_t table_size)") : source.index("void db_incorporate()")]
assert "table_size == 0 ? 1 : table_size" in helper
assert "if (table == NULL)" in helper

for name in (
    "constraint_tab",
    "argument_type_table",
    "child_class_table",
    "charset_writing_direction_table",
    "charset_parsing_direction_table",
    "charset_character_size_table",
    "charset_lang_codes_table",
    "argument_enumset_table",
    "related_argument_table",
    "uil_gadget_variants",
    "uil_urm_nondialog_class",
    "uil_urm_subtree_resource",
    "enumval_values_table",
):
    assignment = source.index(name + " =", source.index("void db_incorporate()"))
    read = source.index("db_read_table", assignment)
    assert "db_allocate_table" in source[assignment:read], name
    assert f"if ({name} == NULL)" in source[assignment:read], name
    assert "goto db_incorporate_done;" in source[assignment:read], name
    assert "goto db_incorporate_done;" in source[read : read + 180], name

print("UIL raw database tables allocate a readable target before fread")
