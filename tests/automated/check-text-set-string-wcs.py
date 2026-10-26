#!/usr/bin/env python3
"""Ensure XmTextSetStringWcs validates input and conversion storage."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("void \nXmTextSetStringWcs")
end = text.index("\nXmTextPosition \nXmTextGetTopCharacter", start)
block = text[start:end]

assert "if (wc_value == NULL)" in block
assert "num_chars == INT_MAX" in block
assert "UINT_MAX / (size_t)tw->text.char_size" in block
assert "tmp = XtMalloc((Cardinal)allocation_bytes);" in block
assert "if (tmp == NULL)" in block
assert "wcstombs(tmp, wc_value, allocation_bytes)" in block

print("text wide-string setter guards: ok")
