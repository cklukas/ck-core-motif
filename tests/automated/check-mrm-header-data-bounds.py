#!/usr/bin/env python3
"""Check Mrm header data-entry reads and writes validate storage bounds."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c").read_text(encoding="utf-8")
checks = {
    "Idb__HDR_GetDataEntry": (
        "data_entry.item_offs > IDBHeaderFreeMax - sizeof(IDBDataEntryHdr)",
        "! Idb__HDR_ValidRecord(bufptr)",
        "datahdr->entry_size >",
    ),
    "Idb__HDR_PutDataEntry": (
        "file_id == NULL || data_entry == NULL",
        "recptr->free_ptr > IDBHeaderFreeMax",
        "entsiz > IDBHeaderFreeMax - recptr->free_ptr",
    ),
}
for function, guards in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    for guard in guards:
        assert guard in region, (function, guard)
print("Mrm header data-entry helpers validate storage bounds")
