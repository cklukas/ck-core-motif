#!/usr/bin/env python3
"""Check XPM hash-table growth rejects size multiplication overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nHashTableGrows")
body = source[start:source.index("/*\n * xpmHashIntern", start)]
assert "size == 0 || size > UINT_MAX / 2" in body
assert "HASH_TABLE_GROWS" in body
assert "size >= UINT_MAX / sizeof(*atomTable)" in body
print("PASS XPM hash-table growth bounds size multiplication")
