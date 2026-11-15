#!/usr/bin/env python3
"""Check non-empty plain text-property table allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("if ((size_t)elements > SIZE_MAX / sizeof(XmString) ||", source.index("/* First count up how many string elements"))
body = source[start:source.index("/* The property length bounds the last element too", start)]
assert "(size_t)elements > SIZE_MAX / sizeof(XmString)" in body
assert "(Cardinal)(sizeof(XmString) *" in body
assert "string_table =" in body
assert "if (string_table == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS non-empty plain text-property table allocation is guarded")
