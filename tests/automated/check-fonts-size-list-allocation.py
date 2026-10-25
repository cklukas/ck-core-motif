#!/usr/bin/env python3
"""Check FontS size and alternate-family list allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "strs == NULL)\n\treturn;" in source
assert "strs == NULL && num != 0" in source
assert "UINT_MAX / sizeof(*strs)" in source
assert "XtMalloc((Cardinal)(sizeof(XmString) *" in source

start = source.index("SetOtherList(XmFontSelectorWidget fsw, Boolean force)")
body = source[start:source.index("/************************************************************", start)]
assert "num < 0" in body
assert "SIZE_MAX / sizeof(*strs)" in body
assert "UINT_MAX / sizeof(*strs)" in body
assert "XtMalloc((Cardinal)(sizeof(XmString) *" in body
