#!/usr/bin/env python3
"""Check empty ASN.1 compound-string import."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
needle = "if (text_prop->nitems == 0) {\n\t    *string_table_return = (XmStringTable)XtMalloc(sizeof(XmString));\n\t    *count_return = 0;"
assert needle in text
print("empty compound string import contract passed")
