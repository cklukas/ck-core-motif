#!/usr/bin/env python3
"""Check start-copy aborts on every clipboard lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartCopy(")
end = source.index("int \nXmClipboardCopy(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard start-copy aborts on lock failure")
