#!/usr/bin/env python3
"""Check clipboard start-copy uses an explicit Xt allocation width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartCopy(")
end = source.index("int \nXmClipboardCopy(", start)
body = source[start:end]

assert "XtMalloc((Cardinal)itemlength)" in body
assert "XtMalloc( (size_t) itemlength )" not in body

print("Clipboard start-copy allocation width is explicit")
