#!/usr/bin/env python3
"""Check Mrm header helpers validate handles and allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c").read_text(encoding="utf-8")
assert "(buffer) != NULL && (buffer)->IDB_record != NULL" in source
checks = {
    "Idb__HDR_GetHeader": "if (file_id == NULL)",
    "Idb__HDR_PutHeader": "if (file_id == NULL)",
    "Idb__HDR_UpdateHeader": "if (file_id == NULL || buffer == NULL)",
    "Idb__HDR_ReturnItem": "if (file_id == NULL || entry_return == NULL)",
    "Idb__HDR_InitHeader": "hdrbuf == NULL || hdrbuf->IDB_record == NULL",
}
for function, guard in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert guard in region, (function, guard)
print("Mrm header helpers validate handles and allocations")
