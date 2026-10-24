#!/usr/bin/env python3
"""Check Mrm RID-map insertion and return validate stored data handles."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIrid.c"
text = source.read_text(encoding="utf-8")
if text.count("IDBDataFreeMax - sizeof(IDBDataEntryHdr)") < 2:
    raise SystemExit("Mrm RID-map handle checks are incomplete")
for name in ("Idb__RID_EnterItem", "Idb__RID_ReturnItem"):
    start = text.index("\n" + name + " (")
    end = text.find("\n", start + 1)
    region = text[start:] if end < 0 else text[start:end]
    if "IDBDataFreeMax - sizeof(IDBDataEntryHdr)" not in region:
        raise SystemExit(f"{name} lacks data-record handle bounds")
print("Mrm RID-map insertion and return validate data handles")
