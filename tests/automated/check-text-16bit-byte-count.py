#!/usr/bin/env python3
"""Check Text 16-bit conversion bounds its emitted-byte count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmTextCharactersToBytes(")
end = source.index("\nchar *", start)
body = source[start:end]
assert "if (count == INT_MAX)" in body
assert "XtFree ((char*)temp_char);" in body
assert "byte_ptr++; count++;" in body

print("Text 16-bit conversion bounds emitted-byte counts")
