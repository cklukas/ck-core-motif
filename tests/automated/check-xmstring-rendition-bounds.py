#!/usr/bin/env python3
"""Check bounded rendition-tag array sizing in XmString segment parsing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "(size_t)*tag_count > SIZE_MAX / sizeof(XmStringTag)",
    "*tag_count = 0;",
    "result = FALSE;",
):
    if text.count(needle) < 2:
        raise SystemExit(f"XmString rendition capacity guard missing twice: {needle}")

print("XmString rendition-tag allocation sizing is bounded")
