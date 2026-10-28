#!/usr/bin/env python3
"""Check clipboard format-atom allocation fits Xt's Cardinal size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Atom \nClipboardGetAtomFromFormat(")
end = source.index("static int \nClipboardGetLenFromFormat(", start)
body = source[start:end]

assert "size_t name_bytes;" in body
assert "name_bytes = format_name_length + prefix_length + 1;" in body
assert "if (name_bytes > UINT_MAX)" in body
assert "XtMalloc((Cardinal)name_bytes)" in body

print("Clipboard format-atom allocation is Cardinal-bounded")
