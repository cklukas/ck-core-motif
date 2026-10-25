#!/usr/bin/env python3
"""Check _XmStringSingleSegment validates output pointers before use."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()

assert "if (pTextOut == NULL || pTagOut == NULL)\n    return FALSE;" in source
start = source.index("_XmStringSingleSegment")
assert source.index("if (pTextOut == NULL || pTagOut == NULL)", start) < source.index(
    "*pTextOut = NULL;", start
)

print("PASS XmString single-segment output pointers are validated")
