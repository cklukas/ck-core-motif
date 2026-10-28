#!/usr/bin/env python3
"""Check clipboard cancellation aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCancelCopy(")
end = source.index("int \nXmClipboardWithdrawFormat(", start)
body = source[start:end]

assert "status = ClipboardLock( display, window);" in body
assert "if ( status != ClipboardSuccess )" in body
assert "return(status);" in body

print("Clipboard cancellation aborts on lock failure")
