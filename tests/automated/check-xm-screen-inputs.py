#!/usr/bin/env python3
"""Check XmGetXmScreen rejects malformed display/screen state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmGetXmScreen(")
end = source.index("\n}\n", start)
region = source[start:end]
assert "num_children != 0 && children == NULL" in region
assert "if (i == ScreenCount(XtDisplay(xmDisplay)))" in region
assert region.index("XtCreateWidget") > region.index("if (i == ScreenCount")
print("PASS XmGetXmScreen validates child and screen state")
