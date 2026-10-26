#!/usr/bin/env python3
"""Ensure virtual-key binding files append with tracked offsets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("Boolean \n_XmVirtKeysLoadFileBindings")
body = source[start:source.index("static void \nLoadVendorBindings", start)]

assert "memcpy(*binding + count, line, line_length);" in body
assert "(*binding)[count] = '\\0';" in body
assert "strcat(*binding, line);" not in body
print("PASS virtual-key binding loading appends in linear time")
