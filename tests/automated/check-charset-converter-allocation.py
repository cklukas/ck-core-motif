#!/usr/bin/env python3
"""Check that the character-set table converter handles both allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToCharSetTable(")
end = text.index("static Boolean\nCvtStringToBooleanDimension", start)
block = text[start:end]

for needle in (
    "if (work_str == NULL)",
    "if (charsetTable == NULL)",
    "XtFree(work_str);",
):
    if needle not in block:
        raise SystemExit(f"character-set converter is missing {needle}")

print("character-set table conversion handles allocation failure")
