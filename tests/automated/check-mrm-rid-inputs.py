#!/usr/bin/env python3
"""Check Mrm resource-ID helpers validate handles and allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIrid.c").read_text(encoding="utf-8")
assert "(buffer) != NULL && (buffer)->IDB_record != NULL" in source
checks = {
    "Idb__RID_EnterItem": "if (file_id == NULL)",
    "Idb__RID_ReturnItem": "if (file_id == NULL || entry_return == NULL)",
    "Idb__RID_NextRID": "if (file_id == NULL || res_id_return == NULL)",
    "Idb__RID_AddRecord": "bufptr == NULL || bufptr->IDB_record == NULL",
}
for function, guard in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert guard in region, (function, guard)
print("Mrm resource-ID helpers validate inputs and allocations")
