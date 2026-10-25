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
    "comma_count > (size_t)UINT_MAX / sizeof(XmButtonType) - 2",
    "in_str_size > (size_t)UINT_MAX",
    "(Cardinal)(sizeof(XmButtonType) *",
    "XtMalloc((Cardinal)in_str_size)",
    "XtFree((char *) buttonTable);",
):
    if needle not in block:
        raise SystemExit(f"button-type converter is missing {needle}")

print("button-type conversion handles allocation failure")
