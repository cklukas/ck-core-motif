#!/usr/bin/env python3
"""Check generated clipboard IDs are validated before serialization."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartCopy(")
end = source.index("int \nXmClipboardCopy(", start)
body = source[start:end]

assert "if (loc_itemid <= 0)" in body
assert "itemId label_itemid;" in body
assert "if (label_itemid <= 0)" in body
assert "dataItemLabelId = (unsigned long)label_itemid;" in body

print("Clipboard generated IDs are validated before serialization")
