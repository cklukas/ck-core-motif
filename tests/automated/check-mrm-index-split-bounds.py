#!/usr/bin/env python3
"""Check Mrm index splitting validates pivots and allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
checks = {
    "Idb__INX_SplitLeafRecord": (
        "gt_hdrptr->index_count < 2",
        "old_itmvec[p_index].index_stg >= IDBIndexLeafFreeMax",
        "lt_buffer == NULL || lt_buffer->IDB_record == NULL",
    ),
    "Idb__INX_SplitNodeRecord": (
        "gt_hdrptr->index_count < 2",
        "old_itmvec[p_index].index_stg >= IDBIndexNodeFreeMax",
        "lt_buffer == NULL || lt_buffer->IDB_record == NULL",
    ),
}
for function, guards in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    for guard in guards:
        assert guard in region, (function, guard)
assert "start != end + 1" in source
print("Mrm index splitting validates pivots and allocations")
