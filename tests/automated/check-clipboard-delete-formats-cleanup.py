#!/usr/bin/env python3
"""Check format deletion frees its data-item buffer on format failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("ClipboardDeleteFormats(")
start = source.index("ClipboardDeleteFormats(", start + 1)
end = source.index("/*---------------------------------------------*/", start)
body = source[start:end]

assert "XtFree( (char *)formatdata);" in body
assert "XtFree( (char *)datalist);" in body
assert "return;" in body

print("Clipboard format deletion cleans up data-item buffers on failure")
