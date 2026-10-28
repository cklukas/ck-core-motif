#!/usr/bin/env python3
"""Check clipboard mark updates release their retrieved item buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("ClipboardMarkItem(")
start = source.index("ClipboardMarkItem(", start + 1)
end = source.index("/*---------------------------------------------*/", start)
body = source[start:end]

assert "ClipboardReplaceItem(display, dataitemid" in body
assert "XtFree((char *)itemheader);" in body

print("Clipboard mark updates release retrieved item buffers")
