#!/usr/bin/env python3
"""Check that optimized XmString cloning validates size and allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "byte_count < TEXT_BYTES_IN_STRUCT",
    "byte_count - TEXT_BYTES_IN_STRUCT > SIZE_MAX - sizeof(_XmStringOptRec)",
    "if (n_o_string == NULL)",
):
    if needle not in text:
        raise SystemExit(f"optimized clone guard is missing: {needle}")

print("XmString optimized clone allocation guards present")
