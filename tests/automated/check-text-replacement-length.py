#!/usr/bin/env python3
"""Check ordinary Text replacement bounds strlen before int conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFunc.c").read_text()
start = source.index("void\n_XmTextReplace")
end = source.index("\nvoid\nXmTextReplace", start)
body = source[start:end]

assert "value_length = strlen(value);" in body
assert "if (value_length > (size_t)INT_MAX)" in body
assert "block.length = (int)value_length;" in body

print("Ordinary Text replacement lengths are bounded")
