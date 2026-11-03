#!/usr/bin/env python3
"""Check WML database string-length tables reject unsafe allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmldbcreate.c").read_text(
    encoding="utf-8"
)

start = source.index("void emit_length_and_string")
start = source.index("void emit_length_and_string", start + 1)
end = source.index("void emit_shorts", start + 1)
body = source[start:end]
assert "header.num_items < 0" in body
assert "(size_t)header.num_items > (size_t)-1 / sizeof(int) - 1" in body
assert "length_count = (size_t)header.num_items + 1;" in body
assert "lengths = (int *) malloc (sizeof (*lengths) * length_count);" in body
assert "if ( lengths == NULL )" in body
assert body.index("emit_header(&header);") > body.index("if ( lengths == NULL )")
assert "if ( string_length >= INT_MAX )" in body
assert "lengths[i] = (int)string_length + 1;" in body

print("PASS WML database string lengths validate allocation and narrowing")
