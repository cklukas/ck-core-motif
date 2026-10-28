#!/usr/bin/env python3
"""Check clipboard retrieval lengths are not narrowed to int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardRetrieveItem(")
end = source.index("static void \nClipboardReplaceItem(", start)
body = source[start:end]

assert "unsigned long add_length" in body
assert "if (*outlength > UINT_MAX)" in body
assert "XtMalloc((Cardinal)*outlength)" in body
assert "(int) length" not in source

print("Clipboard retrieval preserves unsigned length width")
