#!/usr/bin/env python3
"""Check undo-copy aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardUndoCopy(")
end = source.index("int \nXmClipboardStartRetrieve(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard undo-copy aborts on lock failure")
