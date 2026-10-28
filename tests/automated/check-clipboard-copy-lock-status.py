#!/usr/bin/env python3
"""Check clipboard copy aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCopy(")
end = source.index("int \nXmClipboardCancelCopy(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard copy aborts on lock failure")
