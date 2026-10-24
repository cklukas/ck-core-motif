#!/usr/bin/env python3
"""Check bounded dynamic representation-type table sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "DynamicRepTypeNumRecords == UINT_MAX",
    "SIZE_MAX / sizeof(XmRepTypeEntryRec)",
    "DynamicRepTypeNumRecords > UINT_MAX - StandardNumRecs",
    "TotalEntries == UINT_MAX",
    "return NULL;",
):
    if needle not in text:
        raise SystemExit(f"representation-type capacity guard missing: {needle}")

print("representation-type table sizing is bounded")
