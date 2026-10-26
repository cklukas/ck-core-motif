#!/usr/bin/env python3
"""Ensure text-list conversion copies into its pre-sized result linearly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(")
body = source[start:source.index("void\n_XmConvertComplete", start)]

assert "size_t value_offset = 0;" in body
assert "memcpy(total_value + value_offset, values[i], value_size);" in body
assert "value_offset += value_size;" in body
assert "total_value[value_offset] = '\\0';" in body
assert "strcat(total_value, values[i])" not in body
print("PASS text selection conversion uses linear bounded concatenation")
