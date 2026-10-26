#!/usr/bin/env python3
"""Check that hash bucket indexing preserves native hash width."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text(
    encoding="utf-8"
)
start = text.index("static Cardinal\nHashIndex")
body = text[start : text.index("}\n", start) + 2]
expected = "return (Cardinal)((unsigned long)hash % size);"
if expected not in body:
    raise SystemExit("hash index does not preserve native width")
if "(unsigned int)hash" in body:
    raise SystemExit("hash index still truncates to unsigned int")

source = text
assert "static XmConst Cardinal size_table[]" in source
assert "Cardinal i, index;" in source
assert "Cardinal oldsize;" in source
assert "XmHashValue hash;\n  Cardinal index;" in source
assert "int i, index;" not in source
assert "size_table[i] > (size_t)UINT_MAX / sizeof(*new_buckets)" in source
assert "size_table[i] > SIZE_MAX / sizeof(*new_buckets)" not in source

print("hash index preserves native width")
