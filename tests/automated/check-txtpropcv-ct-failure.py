#!/usr/bin/env python3
"""Check compound-text export rejects failed XmString conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("case XmSTYLE_COMPOUND_TEXT:")
end = source.index("      /* Generate the resulting", start)
body = source[start:end]

assert "compound_text[i] = XmCvtXmStringToCT(string_table[i]);" in body
assert "if (compound_text[i] == NULL)" in body
assert "XtFree(compound_text[cleanup]);" in body
assert "return BadAlloc;" in body

print("compound-text export rejects failed XmString conversion")
