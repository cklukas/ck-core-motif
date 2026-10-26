#!/usr/bin/env python3
"""Check Text wide search conversion keeps a size_t result until validation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFind.c").read_text()
start = source.index("XmTextFindStringWcs(")
body = source[start:]
assert "size_t converted;" in body
assert "converted = wcstombs(string, wc_string, allocation_size);" in body
assert "converted != (size_t)-1 && converted <= (size_t)INT_MAX" in body
assert "int wcs_ret_val" not in body

print("Text wide search conversion keeps checked sizes")
