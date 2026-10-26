#!/usr/bin/env python3
"""Ensure CommandPalette assembles display labels without rescanning them."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("static void rebuild")
body = source[start:source.index("static void natural", start)]

assert "memcpy(combined, label, label_length);" in body
assert "memcpy(combined + label_length + 2, shortcut, shortcut_length);" in body
assert "combined[length - 1] = '\\0';" in body
assert "strcat(combined" not in body
print("PASS CommandPalette label assembly uses linear copies")
