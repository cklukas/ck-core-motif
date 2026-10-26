#!/usr/bin/env python3
"""Check TextIn selection-array ownership before freeing old storage."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text()
start = source.index("static void \nInputSetValues(")
end = source.index("\n\nstatic void \nInputDestroy", start)
body = source[start:end]

assert body.count("if (temp_ptr != (XtPointer)sarray)") == 3
assert "XtFree((char *)temp_ptr);" in body
assert "data->sarray = (XmTextScanType *)sarray;" in body

print("TextIn selection-array fallback ownership is checked")
