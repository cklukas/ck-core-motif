#!/usr/bin/env python3
"""Check bounded accumulation when joining text-property items."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text(encoding="utf-8")

if "size_t length = 0;" not in text:
    raise SystemExit("text-property concatenation still uses a narrow length")
if "item_length > SIZE_MAX - length" not in text:
    raise SystemExit("text-property concatenation overflow guard is missing")
if "XtMalloc(length + 1)" not in text:
    raise SystemExit("text-property concatenation does not use checked size")

print("text-property concatenation length accounting is bounded")
