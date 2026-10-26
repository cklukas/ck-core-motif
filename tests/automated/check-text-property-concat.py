#!/usr/bin/env python3
"""Ensure compound-text conversion assembles segments with offsets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
first = source.index("if (count > 1)")
second = source.index("size_t old_size = size_so_far", first)
body = source[first:]

assert "memcpy(newstring + offset, textlist[i], item_length);" in body
assert "newstring[offset] = '\\0';" in body
assert "memcpy(final_string + old_size, text, text_length);" in body
assert "strcat(newstring, textlist[i])" not in body
assert "strcat(final_string, text)" not in body
assert first < second
print("PASS compound-text conversion uses linear segment copies")
