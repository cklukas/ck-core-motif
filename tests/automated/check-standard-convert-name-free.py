#!/usr/bin/env python3
"""Check XmeStandardConvert releases Xlib-owned WM_NAME data."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("void\nXmeStandardConvert")
end = source.index("Atom\nXmeGetEncodingAtom", start)
body = source[start:end]

conversion = body.index("_XmTextToLocaleText")
release = body.index("XFree((char *)value);", conversion)
assert release > conversion
print("Standard WM_NAME conversion frees Xlib-owned property data")
