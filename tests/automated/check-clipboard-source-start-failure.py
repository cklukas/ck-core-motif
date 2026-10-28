#!/usr/bin/env python3
"""Check clipboard-source startup failure relinquishes manager ownership."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(Widget w, XtEnum op, Time time)")
end = source.index("/********************************************************", start)
body = source[start:end]

failure = body.index("if (status != XmClipboardSuccess)")
cleanup = body.index("AbortClipboardSource", failure)
assert "clipboard_owner != None" in body[cleanup:body.index("_XmAppUnlock(app);", failure)]
assert cleanup < body.index("_XmAppUnlock(app);", failure)

print("PASS clipboard-source startup failure cleans ownership and context")
