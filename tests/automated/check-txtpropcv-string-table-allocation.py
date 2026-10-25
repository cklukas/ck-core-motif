#!/usr/bin/env python3
"""Check compound-text string-table allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
assert "(Cardinal)(sizeof(XmString) * (size_t)*count_return)" in source
start = source.index("if (text_prop->encoding == atoms[XmACOMPOUND_TEXT])")
body = source[start:source.index("/* Now convert each bounded", start)]
assert "string_table =" in body
assert "if (string_table == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS compound-text string-table allocation is guarded")
