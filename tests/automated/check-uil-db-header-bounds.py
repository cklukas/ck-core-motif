#!/usr/bin/env python3
"""Check UIL database headers reject signed table-size/count underflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()

assert "if (header.num_items < 0 || header.table_size < 0)" in source
assert "diag_issue_diagnostic( d_bad_database" in source
assert "goto db_incorporate_done;" in source[source.index("if (header.num_items < 0") : source.index("switch (header.table_id)")]
globals_start = source.index("if ( globals.version < 0 ||")
globals_end = source.index("uil_max_arg = globals.uil_max_arg", globals_start)
globals_region = source[globals_start:globals_end]
for field in (
    "version", "uil_max_arg", "uil_max_charset", "charset_lang_table_max",
    "uil_max_object", "uil_max_reason", "uil_max_enumval", "uil_max_enumset",
    "key_k_keyword_count", "key_k_keyword_max_length", "uil_max_child",
):
    assert f"globals.{field} < 0" in globals_region
assert "fclose (dbfile);" in globals_region
assert "return;" in globals_region
print("UIL database headers reject negative table counts and sizes")
