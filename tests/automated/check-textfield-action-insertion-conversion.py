#!/usr/bin/env python3
"""Check TextField keyboard insertion conversion bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nInsertChar(")
end = source.index("static void \n", source.index("XmStackFree((char *)wc_insert_string", start))
body = source[start:end]

assert "size_t num_chars;" in body
assert "insert_length >= TEXT_MAX_INSERT_SIZE" in body
assert "num_chars == (size_t)-1" in body
assert "num_chars > (size_t)INT_MAX" in body
assert "num_chars < 0" not in body
assert "(int)num_chars" in body

print("PASS TextField action insertion conversion guards")
