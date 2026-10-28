#!/usr/bin/env python3
"""Check clipboard record-type validation checks the property length first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardFindItem(")
end = source.index("static int \nGetWindowProperty(", start)
body = source[start:end]

assert "*outlength < sizeof(long)" in body
assert "ptr == NULL" in body
assert "ptr->header.recordType != rec_type" in body

print("Clipboard record-type validation guards short properties")
