#!/usr/bin/env python3
"""Check TextOut scan conversions preserve mbstowcs result width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
assert source.count("size_t converted;") >= 2
assert source.count("converted = mbstowcs") >= 2
assert source.count("converted == (size_t)-1") >= 2
assert "num_chars = mbstowcs" not in source

print("PASS TextOut conversion guards")
