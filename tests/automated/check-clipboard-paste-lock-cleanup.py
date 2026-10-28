#!/usr/bin/env python3
"""Check copy-by-name lock contention releases the retrieved header."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCopyByName(")
end = source.index("int \nXmClipboardUndoCopy(", start)
body = source[start:end]
branch = body[body.index("status = ClipboardLock"):body.index("locked = 1", body.index("status = ClipboardLock"))]

assert "if ( status == ClipboardLocked )" in branch
assert "XtFree( (char *)root_clipboard_header );" in branch
assert branch.index("XtFree( (char *)root_clipboard_header );") < branch.index("return ClipboardLocked;")

print("Clipboard copy-by-name lock contention releases the header buffer")
