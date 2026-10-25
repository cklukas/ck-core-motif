#!/usr/bin/env python3
"""Check that XmGetXmScreen bounds screen lookups before indexing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(
    encoding="utf-8"
)
start = source.index("Widget \nXmGetXmScreen")
region = source[start:source.index("(void)snprintf(name", start)]
assert "for (; i < ScreenCount(XtDisplay(xmDisplay)); i++)" in region
assert "scr = ScreenOfDisplay(XtDisplay(xmDisplay), i);" in region
assert "i++, scr = ScreenOfDisplay" not in region
print("XmScreen lookup checks the screen count before indexing")
