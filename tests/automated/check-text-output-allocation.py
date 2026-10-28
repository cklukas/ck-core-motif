#!/usr/bin/env python3
"""Check TextOut temporary wide-buffer arithmetic is overflow-safe."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
assert source.count("(size_t)num_chars > SIZE_MAX / sizeof(wchar_t) - 1") >= 2
assert source.count("(size_t)num_chars + 1 > UINT_MAX / sizeof(wchar_t)") >= 2
assert "tmp_wc_size = (num_chars + 1) * sizeof(wchar_t)" not in source

print("PASS TextOut allocation arithmetic guards")
