#!/usr/bin/env python3
"""Check clipboard compound-string label lengths retain unsigned width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardStartCopy(")
end = source.index("int \nXmClipboardCopy(", start)
body = source[start:end]

assert "unsigned int asn1strlen;" in body
assert "(itemId)itemheader->dataItemLabelId" in body
assert "\n    int asn1strlen;" not in body

print("Clipboard label lengths preserve unsigned width")
