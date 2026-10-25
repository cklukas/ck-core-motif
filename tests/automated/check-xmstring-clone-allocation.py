#!/usr/bin/env python3
"""Check that XmString clones validate sizes and clean up failures."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "byte_count < TEXT_BYTES_IN_STRUCT",
    "byte_count - TEXT_BYTES_IN_STRUCT > SIZE_MAX - sizeof(_XmStringOptRec)",
    "if (n_o_string == NULL)",
    "SIZE_MAX / sizeof(*entries)",
    "if (entries == NULL)",
    "_XmStringEntryCopy(_XmStrEntry(string)[i])",
    "_XmStringEntryFree(entries[j]);",
):
    if needle not in text:
        raise SystemExit(f"XmString clone guard is missing: {needle}")

print("PASS XmString optimized and non-optimized clone guards are present")
