#!/usr/bin/env python3
"""Verify WML database emitters reject invalid table selectors before use."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmldbcreate.c").read_text(
    encoding="utf-8"
)

assert "static void invalid_table_id(const char *emitter, int table_id)" in source
assert "wmldbcreate: %s received invalid table id %d\\n" in source
for emitter in (
    "emit_chars",
    "emit_ints_and_string",
    "emit_char_table",
    "emit_length_and_string",
    "emit_shorts",
    "emit_int_and_table_shorts",
    "emit_ints",
):
    start = source.index(f"void {emitter}(", source.index("int main"))
    next_emitter = source.find("\nvoid emit_", start + 1)
    body = source[start:next_emitter if next_emitter != -1 else len(source)]
    assert f'invalid_table_id("{emitter}", table_id);' in body

print("PASS WML database emitters reject invalid table selectors")
