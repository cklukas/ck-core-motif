#!/usr/bin/env python3
"""Check empty text-property conversion returns an initialized string."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("TextPropertyToSingleTextItem(")
end = source.index("\n}\n", start)
body = source[start:end]

empty = body.index("else\n    {")
region = body[empty:]
assert '*text_item = XtNewString("");' in region
assert "if (*text_item == NULL)" in region
assert "return(XNoMemory);" in region

print("empty text-property conversion initializes an empty result")
