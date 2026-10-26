#!/usr/bin/env python3
"""Ensure the shared drag copy helper checks size, input, and allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragCI.h").read_text()
start = source.index("_XmAllocAndCopyChecked")
body = source[start:source.index("#define _XmAllocAndCopy(", start)]
assert "len > (size_t)(Cardinal)-1" in body
assert "data == NULL" in body
assert "copy = XtMalloc" in body
assert "if (copy == NULL)" in body
print("PASS shared drag copy helper validates input and allocation")
