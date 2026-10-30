#!/usr/bin/env python3
"""Check ColorSelector list creation failures return before list operations."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()
start = source.index("read_rgb_file(XmColorSelectorWidget csw")
body = source[start:source.index("static int\nCmpColors", start)]
assert "XmColorS_list(csw) == NULL" in body
assert "XtFree((XtPointer) margs);" in body
assert body.index("XmColorS_list(csw) == NULL") < body.index("XtManageChild(XmColorS_list(csw))")
assert "XmColorS_scrolled_list(csw) != NULL" in body
assert body.index("XmColorS_list(csw) == NULL") < body.index("XtVaSetValues(XmColorS_list(csw)")
assert body.index("XtAddCallback(XmColorS_list(csw)") > body.index("XtVaSetValues(XmColorS_list(csw)")

print("ColorSelector list allocation failures are null-safe")
