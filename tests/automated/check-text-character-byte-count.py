#!/usr/bin/env python3
"""Check Text wide-character conversion bounds its byte-count accumulator."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmTextCharactersToBytes(")
end = source.index("\nchar *", start)
body = source[start:end]
assert "nbytes > INT_MAX - count" in body
assert "count += nbytes;" in body

print("Text wide-character conversion bounds byte counts")
