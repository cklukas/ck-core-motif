#!/usr/bin/env python3
"""Check empty locale/string text-property table allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("if (text_prop->nitems == 0) {", source.index("else if (text_prop->encoding == LOCALE_ATOM)"))
body = source[start:source.index("string_table[0]", start)]
assert "string_table = (XmStringTable)XtMalloc(sizeof(XmString));" in body
assert "if (string_table == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
generated = source.index("string_table[0] = XmStringGenerate", start)
assert "if (string_table[0] == NULL)" in source[generated:source.index("*string_table_return", generated)]
print("PASS empty plain text-property table allocation is guarded")
