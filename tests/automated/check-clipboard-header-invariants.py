#!/usr/bin/env python3
"""Check clipboard headers validate list placement and item capacity."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Boolean\nClipboardHeaderRecordValid(")
end = source.index("/*---------------------------------------------*/", start)
body = source[start:end]

assert "header->maxItems == 0" in body
assert "header->currItems > header->maxItems" in body
assert "list_offset < sizeof(ClipboardHeaderRec)" in body
assert "header->currItems > (length - list_offset) / sizeof(itemId)" in body

print("Clipboard headers validate list placement and item capacity")
