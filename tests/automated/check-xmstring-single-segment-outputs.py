#!/usr/bin/env python3
"""Check _XmStringSingleSegment validates output pointers before use."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert "if (pTextOut == NULL || pTagOut == NULL)\n    return FALSE;" in source
assert source.index("if (pTextOut == NULL || pTagOut == NULL)") < source.index(
    "*pTextOut = NULL;", source.index("_XmStringSingleSegment"))

print("XmString single-segment output pointers are validated")
