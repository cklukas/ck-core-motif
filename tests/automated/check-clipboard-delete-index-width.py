#!/usr/bin/env python3
"""Check clipboard item deletion separates wide traversal and signed search indexes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardDeleteItem(")
end = source.index("static void \nClipboardDeleteMarked(", start)
body = source[start:end]

assert "unsigned long i;" in body
assert "long nextpasteindex;" in body
assert "long saved_nextpasteindex;" in body
assert "nextpasteindex = (long)i - 2L;" in body
assert "nextpasteindex = saved_nextpasteindex;" in body

print("Clipboard item deletion preserves index width")
