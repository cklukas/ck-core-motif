#!/usr/bin/env python3
"""Check Mrm child repair validates node metadata and uses a wide counter."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_FixNodeChildren (")
region = source[start:]
assert "file_id == NULL || p_record <= 0" in region
assert "hdrptr->index_count > IDBIndexNodeFreeMax / IDBIndexNodeEntrySize" in region
assert "MrmCount\t\tndx" in region
print("Mrm child repair validates node metadata and uses a wide counter")
