#!/usr/bin/env python3
"""Check Unicode paragraph reads reject null source blocks."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
start = source.index("static struct _XmTextParagraphCache *\nUnicodeParagraph")
end = source.index("\n  entry = (struct _XmTextParagraphCache *)XtCalloc", start)
body = source[start:end]

assert "block.length > 0 && block.ptr == NULL" in body
assert "XtFree(utf8);" in body
assert "memcpy(utf8 + length, block.ptr" in body

print("Unicode paragraph reads guard null source blocks")
