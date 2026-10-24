#!/usr/bin/env python3
"""Check that XmGetXmScreen scans display screens from index zero."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text()
start = source.index("/* Not found; have to do an implied creation.")
end = source.index("\n\ti = 0;\n\tXtSetArg", start)
region = source[start:end]

assert "i = 0;" in region
assert "for (scr = ScreenOfDisplay(XtDisplay(xmDisplay), i);" in region
print("XmGetXmScreen scans display screens from index zero")
