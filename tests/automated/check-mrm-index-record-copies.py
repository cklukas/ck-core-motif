#!/usr/bin/env python3
"""Check Mrm index-record copy helpers reject missing records."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
checks = {
    "Idb__INX_CopyLeafRecord": "dst_recptr == NULL || src_recptr == NULL",
    "Idb__INX_CopyNodeRecord": "dst_recptr == NULL || src_recptr == NULL",
}
for function, guard in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\n/*", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert guard in region, function
print("Mrm index-record copy helpers reject missing records")
