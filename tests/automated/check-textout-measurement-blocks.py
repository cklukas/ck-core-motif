#!/usr/bin/env python3
"""Check TextOut measurements reject invalid blocks at their boundary."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
for name in ("FindWidth", "FindHeight"):
    start = source.index("static int \n" + name + "(XmTextWidget") + len("static int \n")
    body = source[start:start + 500]
    assert "block == NULL || block->ptr == NULL" in body
    assert "block->length <= 0" in body
    assert "return 0;" in body

print("TextOut measurements guard invalid blocks")
