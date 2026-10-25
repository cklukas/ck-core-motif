#!/usr/bin/env python3
"""Check plain text-property XmString generation failures are reported."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Create XmStrings from each string in the value field */")
body = source[start:source.index("*string_table_return = string_table;", start)]
assert "if (string_table[0] == NULL)" in body
assert "XmString generated = XmStringGenerate" in body
assert "if (generated == NULL)" in body
assert "XmStringFree(string_table[cleanup]);" in body
assert "return BadAlloc;" in body
print("PASS plain text-property XmString failures are reported")
