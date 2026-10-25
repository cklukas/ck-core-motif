#!/usr/bin/env python3
"""Check that pending-item inquiries reject null format names."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("int \nXmClipboardInquirePendingItems(")
end = source.index("int\nXmClipboardRegisterFormat(", start)
region = source[start:end]

assert "if (format == NULL)" in region
assert "return ClipboardBadFormat;" in region
assert region.index("format == NULL") < region.index("_XmDisplayToAppContext")
print("PASS pending-item inquiries validate format names")
