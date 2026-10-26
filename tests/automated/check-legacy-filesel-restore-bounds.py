#!/usr/bin/env python3
"""Ensure legacy file-selection restore concatenation is bounded."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c"
text = source.read_text(encoding="utf-8")
start = text.index("dirLen = strlen( dir)")
end = text.index("\n                if (itemString != NULL)", start)
block = text[start:end]

assert "size_t          dirLen" in text
assert "size_t          maskLen" in text
assert "dirLen > SIZE_MAX - maskLen" in block
assert "dirLen + maskLen > (size_t)UINT_MAX - 1" in block
assert "XtMalloc((Cardinal)(dirLen + maskLen + 1))" in block

print("legacy file-selection restore bounds: ok")
