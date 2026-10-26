#!/usr/bin/env python3
"""Check XmString concat rendition-tag allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringConcatAndFree(")
body = source[start:source.index("\nXmStringCopy(", start)]
assert "end_count < 0" in body
assert "SIZE_MAX / sizeof(XmStringTag)" in body
assert "UINT_MAX / sizeof(XmStringTag)" in body
assert "_XmUnoptSegRendEnds(a_last) == NULL" in body
assert "XmStringFree(a);" in body
assert "XmStringFree(b);" in body
print("PASS XmString rendition-tag allocation failure handling")
