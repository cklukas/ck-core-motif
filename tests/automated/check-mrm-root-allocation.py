#!/usr/bin/env python3
"""Check Mrm root initialization validates allocated record buffers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
for function, record_type in (
    ("Idb__INX_InitRootLeafRecord", "IDBrtIndexLeaf"),
    ("Idb__INX_InitRootNodeRecord", "IDBrtIndexNode"),
):
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert f"Idb__BM_InitRecord (file_id, 0, {record_type}" in region
    assert "bufptr == NULL || bufptr->IDB_record == NULL" in region
print("Mrm root initialization validates allocated record buffers")
