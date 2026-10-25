#!/usr/bin/env python3
"""Check bounded charset-table resource conversion sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")

for needle in (
    "size_t strDataSize = 0;",
    "strDataSize > SIZE_MAX - (itemSize + 1)",
    "if (numCharsets == UINT_MAX)",
    "tableSize = sizeof(XmStringCharSet) * ((size_t)numCharsets + 1);",
    "strDataSize > SIZE_MAX - tableSize",
    "strDataSize > (size_t)UINT_MAX - tableSize",
    "XtMalloc(\n\t(Cardinal)(tableSize + strDataSize))",
):
    if needle not in text:
        raise SystemExit(f"charset-table guard missing: {needle}")

print("charset-table conversion sizing is bounded")
