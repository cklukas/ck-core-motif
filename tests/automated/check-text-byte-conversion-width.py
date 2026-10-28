#!/usr/bin/env python3
"""Check Text byte/character conversion uses explicit size boundaries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmTextBytesToCharacters(")
end = source.index("_XmTextCharactersToBytes(", start)
body = source[start:end]

assert "(size_t)num_chars" in body
assert "(size_t)max_char_size" in body
assert "converted > (size_t)INT_MAX" in body
assert "count = (int)converted;" in body

print("Text byte conversion widths are explicit")
