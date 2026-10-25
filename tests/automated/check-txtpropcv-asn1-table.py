#!/usr/bin/env python3
"""Check non-empty ASN.1 string-table allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Now allocate a string table to put them in */")
body = source[start:source.index("/* Run through again", start)]
assert "string_table = (XmStringTable)XtMalloc" in body
assert "UINT_MAX / sizeof(XmString)" in body
assert "(Cardinal)(sizeof(XmString) *" in body
assert "if (string_table == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS non-empty ASN.1 string-table allocation is guarded")
