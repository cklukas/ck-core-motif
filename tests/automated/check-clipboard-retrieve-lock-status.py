#!/usr/bin/env python3
"""Check clipboard retrieval aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int\nClipboardRetrieve(")
end = source.index("int \nXmClipboardInquireCount(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess ) return status;" in body

print("Clipboard retrieval aborts on lock failure")
