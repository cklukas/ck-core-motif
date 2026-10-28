#!/usr/bin/env python3
"""Check WM_CLASS conversion returns Xt-owned data, not an Xlib buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("} else if (atoms[XmACLASS] == cs -> target)")
end = source.index("} else if (atoms[XmANAME] == cs -> target)", start)
body = source[start:end]

assert "XtMalloc((Cardinal)(cs -> length + 1))" in body
assert "memcpy(owned_value, xlib_value" in body
assert "XFree((char *)xlib_value);" in body
assert "cs -> value = (XtPointer)owned_value;" in body
print("Standard WM_CLASS conversion has explicit ownership")
