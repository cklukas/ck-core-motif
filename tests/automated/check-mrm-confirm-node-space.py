#!/usr/bin/env python3
"""Check Mrm node-space confirmation validates node metadata."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_ConfirmNodeSpace (")
end = source.index("\n\n\f", start)
region = source[start:end]
assert "! Idb__INX_ValidNode(buffer)" in region
assert "hdrptr->index_count > IDBIndexNodeFreeMax / IDBIndexNodeEntrySize" in region
assert "hdrptr->heap_start < hdrptr->index_count * IDBIndexNodeEntrySize" in region
print("Mrm node-space confirmation validates record type and metadata")
