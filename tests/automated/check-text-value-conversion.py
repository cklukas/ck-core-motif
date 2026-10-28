#!/usr/bin/env python3
"""Check Text widget wide-value initialization uses size_t conversion results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("static void \nInitializeHook(")
end = source.index("static void ", start + 1)
body = source[start:end]

assert "size_t n_bytes;" in body
assert "n_bytes != (size_t)-1" in body
assert "n_bytes >= 0" not in body
assert "tmp_value[n_bytes] = 0" in body

print("PASS Text value conversion guard")
