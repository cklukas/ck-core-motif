#!/usr/bin/env python3
"""Check that the KeySym resource converter handles both allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToKeySymTable(")
end = text.index("static void\nCvtStringToCharSetTableDestroy", start)
block = text[start:end]

for needle in (
    "if (keySymTable == NULL)",
    "if (work_str == NULL)",
    "XtFree((char *) keySymTable);",
):
    if needle not in block:
        raise SystemExit(f"KeySym converter is missing {needle}")

print("KeySym resource conversion handles allocation failure")
