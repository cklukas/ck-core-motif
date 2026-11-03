#!/usr/bin/env python3
"""Verify WML database serialization rejects negative write counts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmldbcreate.c").read_text(
    encoding="utf-8"
)

assert "static size_t checked_output_count(int count, const char *emitter," in source
assert "if (count < 0)" in source
assert '"wmldbcreate: %s has negative %s\\n"' in source
assert "checked_output_count(header->table_size, \"emit_header\", \"table size\")" in source
for emitter in (
    "emit_chars",
    "emit_ints_and_string",
    "emit_shorts",
    "emit_int_and_table_shorts",
    "emit_ints",
):
    start = source.index(f"void {emitter}(", source.index("int main"))
    next_emitter = source.find("\nvoid emit_", start + 1)
    body = source[start:next_emitter if next_emitter != -1 else len(source)]
    assert "checked_output_count(header.table_size" in body
    assert f'"{emitter}"' in body
assert "if (table[i].values_cnt > 0)" in source
assert "else if (table[i].values_cnt < 0)" in source
assert '"emit_int_and_table_shorts", "value count"' in source
assert "uil_max_object < 0 || uil_max_object > INT_MAX - 7" in source

key_table_start = source.index("void emit_ints_and_string(", source.index("int main"))
key_table_end = source.index("void emit_char_table(", key_table_start)
key_table_body = source[key_table_start:key_table_end]
assert "item_count > table_size / sizeof(*table)" in key_table_body
assert "memcpy(serialized, table, table_size);" in key_table_body
assert "serialized[i].at_name = NULL;" in key_table_body
assert "fwrite (serialized, table_size, 1, bfile);" in key_table_body

enum_table_start = source.index(
    "void emit_int_and_table_shorts(", source.index("int main")
)
enum_table_end = source.index("void emit_ints(", enum_table_start)
enum_table_body = source[enum_table_start:enum_table_end]
assert "item_count >= table_size / sizeof(*table)" in enum_table_body
assert "memcpy(serialized, table, table_size);" in enum_table_body
assert "serialized[i].values = NULL;" in enum_table_body
assert "fwrite (serialized, table_size, 1, bfile);" in enum_table_body

print("PASS WML database serialization validates counts and key-table pointers")
