#!/usr/bin/env python3
"""Check XmTextFindStringWcs bounds its wide-search conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFind.c").read_text()
start = source.index("XmTextFindStringWcs(")
body = source[start:]

assert "size_t num_chars = 0;" in body
assert "num_chars + 1 > SIZE_MAX / (size_t)tw->text.char_size" in body
assert "> (size_t)UINT_MAX" in body
assert "if (string == NULL)" in body
assert "wcstombs(string, wc_string,\n                           allocation_size)" in body
print("PASS XmTextFindStringWcs bounds conversion allocation")
