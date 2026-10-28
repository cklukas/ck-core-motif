#!/usr/bin/env python3
"""Check clipboard replacement chunks fit XChangeProperty's int count."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardReplaceItem(")
end = source.index("static Atom \nClipboardGetAtomFromId(", start)
body = source[start:end]

assert "unsigned long max_req_size;" in body
assert "if (max_req_size > (unsigned long)INT_MAX)" in body
assert "max_req_size = (unsigned long)INT_MAX;" in body
assert "Truncation of next_length" not in body

print("Clipboard replacement request size is int-bounded")
