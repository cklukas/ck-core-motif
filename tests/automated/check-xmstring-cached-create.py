#!/usr/bin/env python3
"""Check cached XmString constructors handle allocation and rollover safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
direction = source[source.index("XmStringDirectionCreate("):source.index(
    "XmStringSeparatorCreate(", source.index("XmStringDirectionCreate("))]
separator = source[source.index("XmStringSeparatorCreate("):source.index(
    "StringTabCreate(", source.index("XmStringSeparatorCreate("))]

assert "if (opt_str == NULL)" in direction
assert "XmStringFree(old_opt_str)" in direction
assert "if (str == NULL)" in separator
assert "XmStringFree(old_str)" in separator

print("cached XmString constructors handle failure and rollover")
