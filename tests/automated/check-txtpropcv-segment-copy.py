#!/usr/bin/env python3
"""Check bounded compound-text segment allocation handles failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Now convert each bounded compound-text segment")
body = source[start:source.index("*string_table_return = string_table;", start)]
assert "segment = (char *)XtMalloc(segment_length + 1);" in body
assert "if (segment == NULL)" in body
assert "XmStringFree(string_table[cleanup]);" in body
assert "XtFree((char *)string_table);" in body
assert "return BadAlloc;" in body
print("PASS bounded compound-text segment allocation is guarded")
