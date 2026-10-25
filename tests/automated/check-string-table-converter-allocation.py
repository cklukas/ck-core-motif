#!/usr/bin/env python3
"""Check string-table conversion allocation and cleanup guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToStringTable(")
end = text.index("static void\nStringCvtDestroy", start)
block = text[start:end]

for needle in (
    "if (table == NULL)",
    "String *new_table",
    "if (new_table == NULL)",
    "if (table[i] == NULL)",
    "span > (size_t)INT_MAX - 1",
    "table[i] = XtMalloc((Cardinal)(span + 1));",
    "XtFree((char *)table[cleanup]);",
    "XtFree((char *)table[i]);",
):
    if needle not in block:
        raise SystemExit(f"string-table converter is missing {needle}")

print("string-table conversion handles allocation failure")
