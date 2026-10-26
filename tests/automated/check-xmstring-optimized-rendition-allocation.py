#!/usr/bin/env python3
"""Check optimized XmString rendition allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("if (_XmEntryRendEnds((_XmStringEntry)a_last) == NULL)")
body = source[start:source.index("} else if (modify_b)", start)]
assert "XtMalloc(sizeof(XmStringTag))" in body
assert "_XmUnoptSegRendEnds(a_last) == NULL" in body
assert "XmStringFree(a);" in body
assert "XmStringFree(b);" in body
print("PASS optimized XmString rendition allocation failure handling")
