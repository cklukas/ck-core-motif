#!/usr/bin/env python3
"""Check clipboard format-count narrowing is validated and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static ClipboardFormatItem \nClipboardFindFormat(")
end = source.index("static void \nClipboardDeleteFormat(", start)
body = source[start:end]

assert "queryitem->cancelledFormatCount < 0" in body
assert "queryitem->cancelledFormatCount > queryitem->formatCount" in body
assert "available_count > INT_MAX ? INT_MAX" in body

print("Clipboard format counts are validated and bounded")
