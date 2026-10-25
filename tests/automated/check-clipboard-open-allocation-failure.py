#!/usr/bin/env python3
"""Check that ClipboardOpen handles a failed header allocation safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static ClipboardHeader \nClipboardOpen(")
end = source.index("static void \nClipboardClose(", start)
region = source[start:end]

assert "root_clipboard_header = NULL;" in region
assert "if (root_clipboard_header == NULL)\n\treturn NULL;" in region
assert "root_clipboard_header->recordType" in region
print("PASS clipboard open handles header allocation failure")
