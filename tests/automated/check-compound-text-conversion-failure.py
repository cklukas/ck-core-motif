#!/usr/bin/env python3
"""Check that failed COMPOUND_TEXT segments reject the whole conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("XmCvtTextPropertyToXmStringTable(")
start = source.index("tempstr = XmCvtCTToXmString(segment);", start)
end = source.index("string_table[i] = tempstr;", start)
region = source[start:end]

assert "if (tempstr == NULL)" in region
assert "XmStringFree(string_table[cleanup])" in region
assert "return XConverterNotFound;" in region

print("compound-text conversion failures reject the table")
