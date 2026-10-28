#!/usr/bin/env python3
"""Check next-item ID buffers are released after property replacement."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static itemId\nClipboardGetNewItemId(")
end = source.index("static void \nClipboardSetAccess(", start)
body = source[start:end]

replace = body.index("ClipboardReplaceItem(")
release = body.index("XtFree( (char *)propertynumber );", replace)
assert release < body.index("return loc_id;", release)

print("Clipboard next-item ID buffer is released")
