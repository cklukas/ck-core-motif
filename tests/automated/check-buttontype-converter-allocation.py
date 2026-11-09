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

trailing_token_failure = """if (i != comma_count + 1)
        {
        XtDisplayStringConversionWarning(display, in_str, XmRButtonType);
        XtFree((char *) buttonTable);
        XtFree((char *) work_str);
        return(FALSE);
        }"""
if trailing_token_failure not in block:
    raise SystemExit("button-type converter leaks its mutable input on token mismatch")

print("button-type conversion handles allocation and token-mismatch cleanup")
