#!/usr/bin/env python3
"""Check XmString concat checks text and segment XtRealloc results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringConcatAndFree(")
body = source[start:source.index("\nXmStringCopy(", start)]
assert "resized_last = (_XmStringNREntry)XtRealloc" in body
assert "resized_text = XtRealloc" in body
assert body.count("if (resized_last == NULL)") >= 2
assert "if (resized_text == NULL)" in body
print("PASS XmString concat realloc failure handling")
