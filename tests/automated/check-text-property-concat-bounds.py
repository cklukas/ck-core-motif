#!/usr/bin/env python3
"""Check bounded accumulation when joining text-property items."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "size_t length = 0;",
    "item_length > SIZE_MAX - length",
    "XtMalloc(length + 1)",
    "text_length == SIZE_MAX",
    "XtRealloc(final_string, size_so_far + 1)",
    "size_t txt_len",
    "txt_len > SIZE_MAX - 2",
    "XtMalloc((txt_len + 2) * sizeof(char))",
):
    if needle not in text:
        raise SystemExit(f"text-property concatenation guard missing: {needle}")

print("text-property concatenation sizing is bounded")
