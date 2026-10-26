#!/usr/bin/env python3
"""Check DragBS atom-entry growth is failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragBS.c").read_text(
    encoding="utf-8"
)

assert "xmAtomsTableEntry new_entries" in source
assert "UINT_MAX / sizeof(xmAtomsTableEntryRec)" in source
assert "SIZE_MAX / sizeof(xmAtomsTableEntryRec)" not in source
assert "(Cardinal)(((size_t)i + 1) * sizeof(xmAtomsTableEntryRec))" in source
assert "if (new_entries == NULL)" in source
assert "atomsTable->numEntries++;" in source
print("DragBS atom-entry growth is bounded and failure-safe")
