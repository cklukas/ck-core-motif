#!/usr/bin/env python3
"""Check XmTextSetStringWcs terminates converted multibyte text."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("void \nXmTextSetStringWcs(")
end = source.index("\n\nXmTextPosition ", start)
body = source[start:end]

assert "result >= allocation_bytes" in body
assert "tmp[0] = '\\0';" in body
assert "tmp[result] = '\\0';" in body
assert "_XmTextSetString(widget, tmp);" in body

print("XmTextSetStringWcs terminates converted text")
