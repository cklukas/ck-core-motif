#!/usr/bin/env python3
"""Check Mrm header RID insertion validates resource data handles."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__HDR_EnterItem (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
for needle in ("data_entry.rec_no <= 0", "IDBHeaderFreeMax - sizeof(IDBDataEntryHdr)",
               "IDBDataFreeMax - sizeof(IDBDataEntryHdr)"):
    if needle not in region:
        raise SystemExit(f"header RID insertion lacks handle check: {needle}")
if region.index("data_entry.rec_no <= 0") > region.index("recno ="):
    raise SystemExit("header RID insertion checks handle too late")
print("Mrm header RID insertion validates header and data handle bounds")
