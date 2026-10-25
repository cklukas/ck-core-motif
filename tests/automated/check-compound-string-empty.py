#!/usr/bin/env python3
"""Check empty ASN.1 compound-string import."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
needle = "if (text_prop->nitems == 0) {"
assert needle in text
empty_case = text[text.index(needle):text.index("\n    }", text.index(needle))]
assert "XtMalloc(sizeof(XmString))" in empty_case
assert "*count_return = 0;" in empty_case
assert "if (*string_table_return == NULL)" in empty_case
print("empty compound string import contract passed")
