#!/usr/bin/env python3
"""Check empty ASN.1 compound-string table allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("else if (text_prop->encoding == atoms[XmA_MOTIF_COMPOUND_STRING])")
body = source[start:source.index("/* First calculate", start)]
assert "*string_table_return = (XmStringTable)XtMalloc(sizeof(XmString));" in body
assert "if (*string_table_return == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS empty ASN.1 string-table allocation is guarded")
