#!/usr/bin/env python3
"""Check that clipboard retrieval size arithmetic cannot wrap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardRetrieveItem(")
end = source.index("static void \nClipboardReplaceItem(", start)
region = source[start:end]

assert "pointer = NULL;" in region
assert "add_length < 0" in region
assert "loclength > ULONG_MAX - (unsigned long)add_length" in region
assert "return ClipboardFail;" in region
print("PASS clipboard retrieval size arithmetic is bounded")
