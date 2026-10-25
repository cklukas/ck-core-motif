#!/usr/bin/env python3
"""Check hash-table resize always advances the chain predecessor."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
start = source.index("_XmResizeHashTable")
end = source.index("/* Bucket management */", start)
body = source[start:end]

assert "else last = current;" in body
assert "#ifdef FIX_1264" not in body
assert body.index("else last = current;") > body.index("if (index != i)")
print("PASS Xm hash resize preserves mixed bucket chains")
