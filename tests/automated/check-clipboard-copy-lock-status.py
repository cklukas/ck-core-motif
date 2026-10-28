#!/usr/bin/env python3
"""Check copy-by-name aborts when lock acquisition fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCopyByName(")
end = source.index("int \nXmClipboardUndoCopy(", start)
body = source[start:end]
lock = body[body.index("status = ClipboardLock"):body.index("locked = 1", body.index("status = ClipboardLock"))]

assert "if ( status != ClipboardSuccess )" in lock
assert "XtFree( (char *)root_clipboard_header );" in lock
assert "return status;" in lock
assert lock.index("XtFree( (char *)root_clipboard_header );") < lock.index("return status;")

print("Clipboard copy-by-name aborts on lock failure")
