#!/usr/bin/env python3
"""Check Mrm index insertion validates header and entry bounds."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
checks = {
    "Idb__INX_EnterLeafIndex": (
        "hdrptr->heap_start > IDBIndexLeafFreeMax",
        "entry_index > hdrptr->index_count",
        "entndx > itemcnt",
    ),
    "Idb__INX_EnterNodeIndex": (
        "hdrptr->heap_start > IDBIndexNodeFreeMax",
        "order != MrmINDEX_LT && order != MrmINDEX_GT",
        "entndx > itemcnt",
    ),
}
for function, guards in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    for guard in guards:
        assert guard in region, (function, guard)
print("Mrm index insertion validates header and entry bounds")
