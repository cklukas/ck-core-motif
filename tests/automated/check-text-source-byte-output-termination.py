#!/usr/bin/env python3
"""Ensure text-source character-to-byte output is terminated."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/TextStrSo.c").read_text()
start = source.index("_XmTextCharactersToBytes(")
start = source.index("  switch (max_char_size)", start)
end = source.index("  default: {", start)
body = source[start:end]
assert "bytes[count] = '\\0';" in body
assert "*byte_ptr = '\\0';" in body
print("Text-source byte output is terminated for one-byte and 16-bit paths")
