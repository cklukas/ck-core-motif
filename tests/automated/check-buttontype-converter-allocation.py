#!/usr/bin/env python3
"""Check that the button-type converter handles both allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("ConvertStringToButtonType(\n        Display *display")
end = text.index("\nCvtStringToCharSetTableDestroy(", start)
block = text[start:end]

for needle in (
    "if (buttonTable == NULL)",
    "if (work_str == NULL)",
    "XtFree((char *) buttonTable);",
):
    if needle not in block:
        raise SystemExit(f"button-type converter is missing {needle}")

print("button-type conversion handles allocation failure")
