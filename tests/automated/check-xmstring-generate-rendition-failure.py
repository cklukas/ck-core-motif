#!/usr/bin/env python3
"""Check XmStringGenerate reports rendition-wrapping failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString\nXmStringGenerate(")
end = source.index("XtPointer\n_XmStringUngenerate(", start)
body = source[start:end]
assert body.count("XmStringFree(result);\n\t\t    return NULL;") >= 6
assert "if (new_seg == NULL)" in body
assert "if (!new_tags)" in body
assert "count > SIZE_MAX / sizeof(XmStringTag)" in body
print("PASS XmStringGenerate rendition failure propagation")
