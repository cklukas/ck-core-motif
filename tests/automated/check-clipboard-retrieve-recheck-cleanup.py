#!/usr/bin/env python3
"""Check retrieve re-check failures close the header and release the lock."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int\nClipboardRetrieve(")
end = source.index("int \nXmClipboardInquireCount(", start)
body = source[start:end]
marker = "if ( matchformat == 0 )"
failure = body[body.index(marker):body.index("return ClipboardFail;", body.index(marker))]

assert "ClipboardClose( display, header );" in failure
assert "ClipboardUnlock( display, window, 0 );" in failure

print("Clipboard retrieve re-check failures close the header and unlock")
