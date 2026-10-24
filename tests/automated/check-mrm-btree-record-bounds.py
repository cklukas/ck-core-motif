#!/usr/bin/env python3
"""Check Mrm B-tree child lookup validates node records and entry bounds."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_GetBtreeRecord (")
end = source.find("\nCardinal ", start + 1)
region = source[start:] if end < 0 else source[start:end]
for guard in (
    "Idb__INX_ValidNode(*buffer_return)",
    "hdrptr->index_count > IDBIndexNodeFreeMax / IDBIndexNodeEntrySize",
    "entry_index >= hdrptr->index_count",
    "recno <= 0",
):
    assert guard in region, guard
print("Mrm B-tree child lookup validates node and entry bounds")
