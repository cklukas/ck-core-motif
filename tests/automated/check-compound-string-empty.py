#!/usr/bin/env python3
"""Check empty ASN.1 compound-string import."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
start = text.index("if (text_prop->nitems == 0) {", text.index("XmCvtTextPropertyToXmStringTable"))
region = text[start : start + 260]
assert "*string_table_return = (XmStringTable)XtMalloc(sizeof(XmString));" in region
assert "*count_return = 0;" in region
print("empty compound string import contract passed")
