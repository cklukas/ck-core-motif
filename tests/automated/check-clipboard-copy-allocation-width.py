#!/usr/bin/env python3
"""Check clipboard copy allocations are Cardinal-bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCopy(")
end = source.index("int \nXmClipboardCopyByName(", start)
body = source[start:end]

assert "if (formatlength > UINT_MAX)" in body
assert "XtMalloc((Cardinal)formatlength)" in body
assert "if (formatdatalength > UINT_MAX)" in body
assert "XtMalloc((Cardinal)formatdatalength)" in body

print("Clipboard copy allocations are Cardinal-bounded")
