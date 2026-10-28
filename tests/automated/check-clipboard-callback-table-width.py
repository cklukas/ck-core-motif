#!/usr/bin/env python3
"""Check clipboard callback-table growth uses explicit wide byte sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartCopy(")
end = source.index("int \nXmClipboardCopy(", start)
body = source[start:end]

assert "size_t proc_bytes = (size_t)newLimit * sizeof(XmCutPasteProc);" in body
assert "size_t id_bytes = (size_t)newLimit * sizeof(long);" in body
assert "XtMalloc((Cardinal)id_bytes)" in body

print("Clipboard callback-table byte sizes are explicit")
