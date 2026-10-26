#!/usr/bin/env python3
"""Ensure RowActions combines label text with tracked offsets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("static char *button_text")
body = source[start:source.index("static void free_items", start)]

assert "memcpy(combined, left, left_length);" in body
assert "memcpy(combined + left_length + 2, right, right_length);" in body
assert "combined[length - 1] = '\\0';" in body
assert "strcat(combined" not in body
print("PASS RowActions label assembly uses linear copies")
