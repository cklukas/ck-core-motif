#!/usr/bin/env python3
"""Check Text help drops release temporary location data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.rindex("TextDestinationCallback(Widget w")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
assert "cb->dropAction != XmDROP_HELP" in body
assert "XtFree((char *) ds->location_data)" in body
assert "ds->location_data = NULL" in body

print("Text help drops release temporary location data")
