#!/usr/bin/env python3
"""Check serialized clipboard label IDs fit the signed itemId type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardDeleteItemLabel(")
end = source.index("static void \nClipboardDeleteItem(", start)
body = source[start:end]

assert "datalist->dataItemLabelId > (unsigned long)LONG_MAX" in body
assert "(itemId)datalist->dataItemLabelId" in body

print("Clipboard label IDs are bounded before deletion")
