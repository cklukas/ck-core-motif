#!/usr/bin/env python3
"""Ensure XmString segment copies bound character-count arithmetic."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c"
text = source.read_text(encoding="utf-8")
start = text.index("Copy the text.")
end = text.index("\n  /* Free the local context. */", start)
block = text[start:end]

assert "*char_count < 0" in block
assert "text_bytes > SIZE_MAX - 2 * sizeof(wchar_t)" in block
assert "text_bytes + 2 * sizeof(wchar_t) > UINT_MAX" in block
assert "XtMalloc((Cardinal)padded_bytes)" in block

print("XmString segment copy bounds: ok")
