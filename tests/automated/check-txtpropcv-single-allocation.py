#!/usr/bin/env python3
"""Check single-string text-property copying handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("TextPropertyToSingleTextItem(")
body = source[start:source.index("/************************************************************************", start + 1)]
assert "*text_item = XtNewString(textlist[0]);" in body
assert "if (*text_item == NULL)" in body
assert "XFreeStringList(textlist);" in body
assert "return(XNoMemory);" in body
print("PASS single text-property allocation is guarded")
