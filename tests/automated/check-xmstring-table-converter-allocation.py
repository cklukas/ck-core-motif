#!/usr/bin/env python3
"""Check XmString-table conversion allocation and cleanup guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("GetNextXmString(")
end = text.index("\nXmStringCvtDestroy(", start)
block = text[start:end]

for needle in (
    "*cs = NULL;",
    "if (*cs == NULL)",
    "if (table == NULL)",
    "if (new_table == NULL)",
    "if (table[str_no] == NULL)",
    "XmStringFree(table[cleanup]);",
):
    if needle not in block:
        raise SystemExit(f"XmString-table conversion is missing {needle}")

print("XmString-table conversion handles allocation failure")
