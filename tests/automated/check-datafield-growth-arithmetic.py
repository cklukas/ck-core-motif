#!/usr/bin/env python3
"""Check DataField replacement growth arithmetic before reallocating."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("/* Allocate more space if we need it.")
end = source.index("\n  if (XmTextF_has_primary", start)
body = source[start:end]
assert "required_chars" in body
assert "SIZE_MAX - (size_t)insert_length" in body
assert "required_chars > SIZE_MAX / sizeof(wchar_t)" in body
assert "growth_bytes > (size_t)INT_MAX" in body
assert "XmTextF_size_allocd(tf) = (int)growth_bytes" in body

print("DataField replacement growth arithmetic is checked")
