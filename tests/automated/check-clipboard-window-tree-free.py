#!/usr/bin/env python3
"""Check XQueryTree child lists use Xlib deallocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardSearchForWindow(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "XQueryTree" in region
assert "XFree( (char*)children );" in region
assert "XtFree( (char*)children );" not in region
print("clipboard window-tree results are released with XFree")
