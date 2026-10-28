#!/usr/bin/env python3
"""Check clipboard atom names use the allocator returned by Xlib."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardInquireFormat(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "ptr = XGetAtomName" in region
assert "XFree( (char *) ptr );" in region
assert "XtFree( (char *) ptr );" not in region
print("clipboard atom names are released with XFree")
