#!/usr/bin/env python3
"""Check start-retrieve aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartRetrieve(")
end = source.index("int \nXmClipboardEndRetrieve(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard start-retrieve aborts on lock failure")
