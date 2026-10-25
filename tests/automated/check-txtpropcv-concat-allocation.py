#!/usr/bin/env python3
"""Check text-property list concatenation handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("TextPropertyToSingleTextItem(")
body = source[start:source.index("/************************************************************************", start + 1)]
assert "newstring = (char *)XtMalloc(length + 1);" in body
assert "if (newstring == NULL)" in body
assert "XFreeStringList(textlist);" in body
assert "return(XNoMemory);" in body
print("PASS text-property concatenation allocation is guarded")
